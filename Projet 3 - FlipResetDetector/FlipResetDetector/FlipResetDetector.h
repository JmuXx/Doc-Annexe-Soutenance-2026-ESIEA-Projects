#pragma once

#include "GuiBase.h"

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"

#include <array>
#include <memory>


constexpr auto plugin_version =
stringify(VERSION_MAJOR) "."
stringify(VERSION_MINOR) "."
stringify(VERSION_PATCH) "."
stringify(VERSION_BUILD);


class FlipResetDetector :
    public BakkesMod::Plugin::BakkesModPlugin
{
private:

    // Indique si un replay de but est en cours.
    bool isGoalReplay = false;


    // ============================================================
    // Détection du flip reset
    // ============================================================

    // Dernier événement OnHitBall traité.
    // Évite de traiter plusieurs fois le même contact.
    float lastBallHitTime = -1000.0f;


    // Dernier instant de contact avec la balle pour chaque roue.
    std::array<float, 4> lastWheelBallContact =
    {
        -1000.0f,
        -1000.0f,
        -1000.0f,
        -1000.0f
    };


    // Indique qu'un contact avec la balle est en cours d'analyse.
    bool pendingBallHit = false;

    float pendingBallHitTime = -1000.0f;


    // Roues observées pendant la fenêtre d'analyse actuelle.
    std::array<bool, 4> wheelsSeen =
    {
        false,
        false,
        false,
        false
    };


    // Empêche de valider plusieurs fois le même reset.
    bool resetDetectedForCurrentHit = false;


    // ============================================================
    // Interface
    // ============================================================

    // État très court des contacts roue/balle.
    // Utilisé uniquement pour l'affichage.
    std::array<bool, 4> wheelDisplayState =
    {
        false,
        false,
        false,
        false
    };


    // Résultat de la dernière tentative :
    // true = roue ayant touché la balle.
    std::array<bool, 4> lastAttemptWheels =
    {
        false,
        false,
        false,
        false
    };


    // Permet d'afficher les marqueurs en gris
    // tant qu'aucune tentative n'a été analysée.
    bool hasAttemptResult = false;


    // Affichage temporaire du message "FLIP RESET!".
    bool showResetIndicator = false;

    float resetIndicatorStartTime = -1000.0f;


    // Image de la voiture affichée dans l'overlay.
    std::shared_ptr<ImageWrapper> carImage;


    // Position et dimensions de l'overlay.
    int overlayX = 35;
    int overlayY = 250;

    int overlayWidth = 230;
    int overlayHeight = 330;


    // Analyse des contacts des roues à chaque tick physique.
    void updatePhysics(CarWrapper car);

    // Démarre l'analyse lorsqu'un contact voiture/balle est détecté.
    void handleBallHit(CarWrapper car);

    // Dessine l'interface.
    void renderOverlay(CanvasWrapper canvas);


public:

    void onLoad() override;
};