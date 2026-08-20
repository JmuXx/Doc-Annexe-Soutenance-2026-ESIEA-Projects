from z3 import BitVec, BitVecVal, RotateLeft, Solver, UGE, ULE, sat


# Valeurs attendues par la VM après chacun des deux rounds.
ROUND1_TARGETS = [0x82, 0x7D, 0x21, 0xF0]
ROUND2_TARGETS = [0xBF, 0x32, 0x0F, 0x77]


def round1(p):
    """
    Reproduction symbolique du premier round de la VM.
    """

    # Chargement et première transformation des 8 caractères.
    r0 = p[0] ^ p[4]
    r1 = p[1] + p[5]
    r2 = RotateLeft(p[2] ^ p[6], 3)
    r3 = p[3] + p[7]

    # Mélange des quatre registres.
    r0 = r0 + r1
    r1 = r1 ^ r2
    r2 = r2 + r3
    r3 = r3 ^ r0

    # Transformations finales.
    r0 ^= 0xA7
    r1 += 0x3D
    r2 = RotateLeft(r2, 5)
    r3 ^= 0x5C

    return [r0, r1, r2, r3]


def round2(p):
    """
    Reproduction symbolique du second round de la VM.
    """

    # Nouvelle initialisation des registres.
    r0 = RotateLeft(p[0], 1) + p[4]
    r1 = RotateLeft(p[1], 1) + p[5]
    r2 = RotateLeft(p[2], 1) + p[6]
    r3 = RotateLeft(p[3], 1) + p[7]

    # Second mélange.
    r0 ^= r1
    r1 += r2
    r2 ^= r3
    r3 += r0

    # Secondes transformations finales.
    r0 ^= 0x6D
    r1 = RotateLeft(r1, 3)
    r2 += 0x47
    r3 ^= 0xB4

    return [r0, r1, r2, r3]


def solve():
    solver = Solver()

    # Le mot de passe contient exactement 8 octets.
    password = [
        BitVec(f"p{i}", 8)
        for i in range(8)
    ]

    # Le programme n'accepte que des caractères ASCII imprimables.
    for char in password:
        solver.add(UGE(char, 0x21))
        solver.add(ULE(char, 0x7E))

    # Reproduction symbolique des deux rounds.
    regs1 = round1(password)
    regs2 = round2(password)

    # Contraintes de validation.
    for reg, target in zip(regs1, ROUND1_TARGETS):
        solver.add(reg == BitVecVal(target, 8))

    for reg, target in zip(regs2, ROUND2_TARGETS):
        solver.add(reg == BitVecVal(target, 8))

    # Résolution.
    if solver.check() != sat:
        raise RuntimeError("Aucune solution trouvée")

    model = solver.model()

    return "".join(
        chr(model.eval(char).as_long())
        for char in password
    )


if __name__ == "__main__":
    password = solve()
    print(f"Password: {password}")
