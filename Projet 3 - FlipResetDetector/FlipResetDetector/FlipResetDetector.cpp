#include "pch.h"
#include "FlipResetDetector.h"


BAKKESMOD_PLUGIN(
    FlipResetDetector,
    "Flip Reset Detector",
    plugin_version,
    PLUGINTYPE_FREEPLAY
)


std::shared_ptr<CVarManagerWrapper> _globalCvarManager;


// ================================================================
// Chargement du plugin
// ================================================================

void FlipResetDetector::onLoad()
{
    _globalCvarManager = cvarManager;

    LOG("[FlipResetDetector] Plugin loaded!");


    // Image utilisée dans l'overlay.
    carImage =
        std::make_shared<ImageWrapper>(
            gameWrapper->GetDataFolder()
            / "FlipResetDetector"
            / "fennec_under.png",

            true,
            false
        );


    // Callback de rendu appelé à chaque frame.
    gameWrapper->RegisterDrawable(
        [this](CanvasWrapper canvas)
        {
            renderOverlay(canvas);
        }
    );


    // IsInFreeplay() reste vrai pendant certains replays de but,
    // donc un état séparé est utilisé pour les ignorer.
    gameWrapper->HookEvent(
        "Function GameEvent_Soccar_TA.ReplayPlayback.BeginState",

        [this](std::string eventName)
        {
            isGoalReplay = true;

            pendingBallHit = false;

            resetDetectedForCurrentHit = false;
        }
    );


    gameWrapper->HookEvent(
        "Function GameEvent_Soccar_TA.ReplayPlayback.EndState",

        [this](std::string eventName)
        {
            isGoalReplay = false;
        }
    );


    // SetVehicleInput est appelé très régulièrement.
    // Il permet d'observer les contacts de roues autour de OnHitBall.
    gameWrapper->HookEventWithCaller<CarWrapper>(
        "Function TAGame.Car_TA.SetVehicleInput",

        [this](
            CarWrapper car,
            void* params,
            std::string eventName
            )
        {
            updatePhysics(car);
        }
    );


    // Contact général entre la voiture et la balle.
    gameWrapper->HookEventWithCallerPost<CarWrapper>(
        "Function TAGame.Car_TA.OnHitBall",

        [this](
            CarWrapper car,
            void* params,
            std::string eventName
            )
        {
            handleBallHit(car);
        }
    );
}


// ================================================================
// Analyse physique des roues
// ================================================================

void FlipResetDetector::updatePhysics(CarWrapper car)
{
    if (!gameWrapper->IsInFreeplay())
        return;

    if (isGoalReplay)
        return;

    if (car.IsNull())
        return;


    ServerWrapper server =
        gameWrapper->GetGameEventAsServer();


    if (server.IsNull())
        return;


    BallWrapper ball =
        server.GetBall();


    if (ball.IsNull())
        return;


    float now =
        server.GetSecondsElapsed();


    // Adresse de la balle utilisée pour identifier
    // les vrais contacts roue/balle.
    void* ballActor =
        reinterpret_cast<void*>(
            ball.memory_address
            );


    VehicleSimWrapper vehicleSim =
        car.GetVehicleSim();


    if (vehicleSim.IsNull())
        return;


    ArrayWrapper<WheelWrapper> wheels =
        vehicleSim.GetWheels();


    if (wheels.IsNull())
        return;


    int wheelCount =
        wheels.Count();


    if (wheelCount < 4)
        return;


    // Analyse des quatre roues.
    for (int i = 0; i < wheelCount; i++)
    {
        WheelWrapper wheel =
            wheels.Get(i);


        if (wheel.IsNull())
            continue;


        int wheelIndex =
            wheel.GetWheelIndex();


        if (wheelIndex < 0 || wheelIndex >= 4)
            continue;


        WheelContactData contact =
            wheel.GetContact();


        // bHasContact ne suffit pas : la roue peut toucher
        // le terrain. Actor permet de vérifier que l'objet
        // touché est bien la balle.
        bool touchesBall =
            contact.bHasContact != 0 &&
            contact.Actor == ballActor;


        if (touchesBall)
        {
            lastWheelBallContact[wheelIndex] =
                now;


            if (pendingBallHit)
            {
                wheelsSeen[wheelIndex] =
                    true;
            }
        }
    }


    // Petite persistance pour éviter un changement d'état
    // trop brutal dans l'interface.
    constexpr float DISPLAY_PERSISTENCE =
        0.060f;


    for (int i = 0; i < 4; i++)
    {
        wheelDisplayState[i] =
            now - lastWheelBallContact[i]
            <= DISPLAY_PERSISTENCE;
    }


    if (!pendingBallHit)
        return;


    // Fenêtre de 80 ms après OnHitBall.
    // Elle permet de récupérer les contacts des roues qui arrivent
    // légèrement après l'événement principal.
    constexpr float ANALYSIS_WINDOW =
        0.080f;


    float elapsed =
        now - pendingBallHitTime;


    bool allFourWheels =
        wheelsSeen[0] &&
        wheelsSeen[1] &&
        wheelsSeen[2] &&
        wheelsSeen[3];


    bool hasFlip =
        car.HasFlip();


    // Un reset est validé lorsque les quatre roues ont été observées
    // sur la balle et que le flip est disponible.
    if (
        allFourWheels &&
        hasFlip &&
        !resetDetectedForCurrentHit
        )
    {
        resetDetectedForCurrentHit =
            true;


        // Sauvegarde du résultat pour l'interface.
        lastAttemptWheels =
            wheelsSeen;

        hasAttemptResult =
            true;


        showResetIndicator =
            true;

        resetIndicatorStartTime =
            now;


        LOG(
            "[FlipResetDetector] >>> FLIP RESET DETECTED <<<"
        );


        LOG(
            "[FlipResetDetector] "
            "W0={} | "
            "W1={} | "
            "W2={} | "
            "W3={} | "
            "hasFlip={} | "
            "elapsed={:.3f}s",

            wheelsSeen[0],
            wheelsSeen[1],
            wheelsSeen[2],
            wheelsSeen[3],

            hasFlip,

            elapsed
        );
    }


    // Fin de l'analyse du contact.
    if (elapsed > ANALYSIS_WINDOW)
    {
        // Même en cas d'échec, le résultat est conservé
        // pour montrer quelles roues ont manqué le contact.
        if (!resetDetectedForCurrentHit)
        {
            lastAttemptWheels =
                wheelsSeen;

            hasAttemptResult =
                true;


            int wheelCountSeen =
                static_cast<int>(wheelsSeen[0]) +
                static_cast<int>(wheelsSeen[1]) +
                static_cast<int>(wheelsSeen[2]) +
                static_cast<int>(wheelsSeen[3]);


            LOG(
                "[FlipResetDetector] AIR HIT END | "
                "W0={} | "
                "W1={} | "
                "W2={} | "
                "W3={} | "
                "seen={}/4 | "
                "hasFlip={}",

                wheelsSeen[0],
                wheelsSeen[1],
                wheelsSeen[2],
                wheelsSeen[3],

                wheelCountSeen,

                hasFlip
            );
        }


        pendingBallHit =
            false;
    }
}


// ================================================================
// Début de l'analyse d'un contact avec la balle
// ================================================================

void FlipResetDetector::handleBallHit(
    CarWrapper car
)
{
    if (!gameWrapper->IsInFreeplay())
        return;

    if (isGoalReplay)
        return;

    if (car.IsNull())
        return;


    // Ignore les contacts avec le terrain, les murs ou le plafond.
    //
    // GetbOnGround() n'est pas utilisé ici car le contact avec
    // la balle peut lui-même faire passer cet état à true
    // pendant un flip reset.
    int worldWheelContacts =
        car.GetNumWheelWorldContacts();


    if (worldWheelContacts > 0)
        return;


    ServerWrapper server =
        gameWrapper->GetGameEventAsServer();


    if (server.IsNull())
        return;


    float now =
        server.GetSecondsElapsed();


    // OnHitBall peut être déclenché plusieurs fois pour le même
    // contact. Les événements espacés de moins de 50 ms sont ignorés.
    if (
        now - lastBallHitTime
        < 0.050f
        )
    {
        return;
    }


    lastBallHitTime =
        now;


    pendingBallHit =
        true;


    pendingBallHitTime =
        now;


    resetDetectedForCurrentHit =
        false;


    wheelsSeen =
    {
        false,
        false,
        false,
        false
    };


    // Le résultat précédent reste affiché jusqu'à la fin
    // de l'analyse de cette nouvelle tentative.


    // Fenêtre de 50 ms avant OnHitBall pour récupérer les contacts
    // enregistrés légèrement avant l'événement.
    constexpr float PRE_HIT_WINDOW =
        0.050f;


    for (int i = 0; i < 4; i++)
    {
        if (
            now - lastWheelBallContact[i]
            <= PRE_HIT_WINDOW
            )
        {
            wheelsSeen[i] =
                true;
        }
    }


    LOG(
        "[FlipResetDetector] AIR HIT START | "
        "W0={} | "
        "W1={} | "
        "W2={} | "
        "W3={} | "
        "hasFlip={} | "
        "onGround={} | "
        "worldContacts={}",

        wheelsSeen[0],
        wheelsSeen[1],
        wheelsSeen[2],
        wheelsSeen[3],

        car.HasFlip(),
        car.GetbOnGround(),
        worldWheelContacts
    );
}


// ================================================================
// Interface
// ================================================================

void FlipResetDetector::renderOverlay(
    CanvasWrapper canvas
)
{
    if (!gameWrapper->IsInFreeplay())
        return;


    if (isGoalReplay)
        return;


    ServerWrapper server =
        gameWrapper->GetGameEventAsServer();


    // Le message "FLIP RESET!" reste visible pendant une seconde.
    if (
        !server.IsNull() &&
        showResetIndicator
        )
    {
        float now =
            server.GetSecondsElapsed();


        if (
            now - resetIndicatorStartTime
    > 1.0f
            )
        {
            showResetIndicator =
                false;
        }
    }


    // ============================================================
    // Fond de l'overlay
    // ============================================================

    canvas.SetPosition(
        Vector2{
            overlayX,
            overlayY
        }
    );


    canvas.SetColor(
        LinearColor{
            18,
            18,
            18,
            185
        }
    );


    canvas.FillBox(
        Vector2{
            overlayWidth,
            overlayHeight
        }
    );


    // ============================================================
    // Bandeau et titre
    // ============================================================

    constexpr int TITLE_HEIGHT =
        44;


    canvas.SetPosition(
        Vector2{
            overlayX,
            overlayY
        }
    );


    canvas.SetColor(
        LinearColor{
            8,
            8,
            8,
            215
        }
    );


    canvas.FillBox(
        Vector2{
            overlayWidth,
            TITLE_HEIGHT
        }
    );


    const std::string title =
        "FLIP RESET DETECTOR";


    constexpr float TITLE_SCALE_X =
        1.0f;

    constexpr float TITLE_SCALE_Y =
        1.0f;


    Vector2F titleSize =
        canvas.GetStringSize(
            title,
            TITLE_SCALE_X,
            TITLE_SCALE_Y
        );


    // Centrage automatique du titre.
    float titleX =
        static_cast<float>(overlayX) +
        (
            static_cast<float>(overlayWidth) -
            titleSize.X
            ) / 2.0f;


    float titleY =
        static_cast<float>(overlayY) +
        (
            static_cast<float>(TITLE_HEIGHT) -
            titleSize.Y
            ) / 2.0f;


    canvas.SetColor(
        LinearColor{
            255,
            255,
            255,
            255
        }
    );


    canvas.SetPosition(
        Vector2{
            static_cast<int>(titleX),
            static_cast<int>(titleY)
        }
    );


    canvas.DrawString(
        title,
        TITLE_SCALE_X,
        TITLE_SCALE_Y,
        true
    );


    // ============================================================
    // Image de la voiture
    // ============================================================

    if (
        carImage &&
        carImage->IsLoadedForCanvas()
        )
    {
        constexpr float CAR_IMAGE_WIDTH =
            1240.0f;

        constexpr float CAR_IMAGE_HEIGHT =
            1269.0f;


        constexpr float CAR_SCALE =
            0.16f;


        float displayedCarWidth =
            CAR_IMAGE_WIDTH * CAR_SCALE;


        float displayedCarHeight =
            CAR_IMAGE_HEIGHT * CAR_SCALE;


        // Centrage horizontal de la voiture.
        float carX =
            static_cast<float>(overlayX) +
            (
                static_cast<float>(overlayWidth) -
                displayedCarWidth
                ) / 2.0f;


        constexpr float CAR_AREA_TOP =
            50.0f;

        constexpr float CAR_AREA_BOTTOM =
            280.0f;


        float carAreaHeight =
            CAR_AREA_BOTTOM -
            CAR_AREA_TOP;


        float carY =
            static_cast<float>(overlayY) +
            CAR_AREA_TOP +
            (
                carAreaHeight -
                displayedCarHeight
                ) / 2.0f;


        canvas.SetColor(
            LinearColor{
                255,
                255,
                255,
                255
            }
        );


        canvas.SetPosition(
            Vector2{
                static_cast<int>(carX),
                static_cast<int>(carY)
            }
        );


        canvas.DrawTexture(
            carImage.get(),
            CAR_SCALE
        );
    }


    // ============================================================
    // Marqueurs des quatre roues
    // ============================================================

    // Positions réglées manuellement pour correspondre
    // aux roues visibles sur l'image.
    struct WheelGuiPosition
    {
        int x;
        int y;
    };


    WheelGuiPosition wheelPositions[4] =
    {
        // W0
        {
            overlayX + 81,
            overlayY + 98
        },

        // W1
        {
            overlayX + 134,
            overlayY + 98
        },

        // W2
        {
            overlayX + 81,
            overlayY + 199
        },

        // W3
        {
            overlayX + 134,
            overlayY + 199
        }
    };


    for (int i = 0; i < 4; i++)
    {
        canvas.SetPosition(
            Vector2{
                wheelPositions[i].x,
                wheelPositions[i].y
            }
        );


        if (!hasAttemptResult)
        {
            // Gris avant la première tentative.
            canvas.SetColor(
                LinearColor{
                    160,
                    160,
                    160,
                    90
                }
            );
        }
        else if (lastAttemptWheels[i])
        {
            // Vert : la roue a touché la balle.
            canvas.SetColor(
                LinearColor{
                    50,
                    255,
                    80,
                    220
                }
            );
        }
        else
        {
            // Rouge : la roue a manqué le contact.
            canvas.SetColor(
                LinearColor{
                    255,
                    45,
                    45,
                    180
                }
            );
        }


        canvas.FillBox(
            Vector2{
                13,
                34
            }
        );
    }


    // ============================================================
    // Zone de statut
    // ============================================================

    constexpr int STATUS_MARGIN =
        14;

    constexpr int STATUS_HEIGHT =
        29;


    int statusWidth =
        overlayWidth -
        STATUS_MARGIN * 2;


    int statusX =
        overlayX +
        STATUS_MARGIN;


    int statusY =
        overlayY +
        286;


    canvas.SetPosition(
        Vector2{
            statusX,
            statusY
        }
    );


    canvas.SetColor(
        LinearColor{
            0,
            0,
            0,
            130
        }
    );


    canvas.FillBox(
        Vector2{
            statusWidth,
            STATUS_HEIGHT
        }
    );


    // ============================================================
    // Message de reset
    // ============================================================

    if (showResetIndicator)
    {
        const std::string resetText =
            "FLIP RESET!";


        constexpr float RESET_TEXT_SCALE_X =
            1.15f;

        constexpr float RESET_TEXT_SCALE_Y =
            1.15f;


        Vector2F resetTextSize =
            canvas.GetStringSize(
                resetText,
                RESET_TEXT_SCALE_X,
                RESET_TEXT_SCALE_Y
            );


        // Centrage automatique dans la zone de statut.
        float resetTextX =
            static_cast<float>(statusX) +
            (
                static_cast<float>(statusWidth) -
                resetTextSize.X
                ) / 2.0f;


        float resetTextY =
            static_cast<float>(statusY) +
            (
                static_cast<float>(STATUS_HEIGHT) -
                resetTextSize.Y
                ) / 2.0f;


        canvas.SetColor(
            LinearColor{
                255,
                255,
                255,
                255
            }
        );


        canvas.SetPosition(
            Vector2{
                static_cast<int>(resetTextX),
                static_cast<int>(resetTextY)
            }
        );


        canvas.DrawString(
            resetText,
            RESET_TEXT_SCALE_X,
            RESET_TEXT_SCALE_Y,
            true
        );
    }
}