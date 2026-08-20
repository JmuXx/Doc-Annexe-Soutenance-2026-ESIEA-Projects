#include <stdio.h>
#include <string.h>

#include "challenge/challenge2.h"

#include "security/antidebug.h"

#define VM_REG_COUNT 4
#define VM_PROGRAM_MAX 1024
#define VM_MAX_PATCHES 32
#define VM_OPCODE_COUNT 13

#define REWARD_SIZE 29

enum
{
    LOGICAL_LOAD_INPUT = 0,
    LOGICAL_XOR_INPUT,
    LOGICAL_ADD_INPUT,
    LOGICAL_XOR_REG,
    LOGICAL_ADD_REG,
    LOGICAL_XOR_IMM,
    LOGICAL_ADD_IMM,
    LOGICAL_ROL,
    LOGICAL_CMP,
    LOGICAL_JNZ,
    LOGICAL_NOP,
    LOGICAL_SUCCESS,
    LOGICAL_FAIL
};

typedef struct
{
    uint32_t regs[VM_REG_COUNT];
    size_t pc;
    int zero;
    int halted;
    int success;
    const char *input;

} Challenge2VM;

typedef struct
{
    unsigned char data[VM_PROGRAM_MAX];

    size_t size;

    size_t patches[VM_MAX_PATCHES];
    size_t patch_count;

} VMProgram;

typedef struct
{
    unsigned char encoded[VM_OPCODE_COUNT];
    unsigned char decoded[256];

} VMOpcodeMap;

typedef struct
{
    unsigned char encoded[VM_REG_COUNT];

} VMRegisterMap;

static const unsigned char encrypted_reward[REWARD_SIZE] =
{
    0x22,
    0x87,
    0x74,
    0xF0,
    0xCB,
    0x0D,
    0x0A,
    0x36,
    0x51,
    0x2A,
    0x9F,
    0xC6,
    0x58,
    0x53,
    0xFC,
    0x07,
    0xE6,
    0x6A,
    0x71,
    0x0A,
    0x08,
    0x78,
    0xDD,
    0x4D,
    0xC8,
    0x4D,
    0xF1,
    0x64,
    0x49
};

static uint32_t rotl32(
    uint32_t value,
    unsigned int shift)
{
    return
        (value << shift) |
        (value >> (32 - shift));
}

static unsigned char rol8(
    unsigned char value,
    unsigned int shift)
{
    shift &=
        7;

    if (shift == 0)
    {
        return value;
    }

    return
        (unsigned char)(
            (value << shift) |
            (value >> (8 - shift))
        );
}

static int password_charset_valid(
    const char *password)
{
    for (int i = 0; i < 8; i++)
    {
        unsigned char c =
            (unsigned char)password[i];

        if (c < 33 ||
            c > 126)
        {
            return 0;
        }
    }

    return 1;
}

static uint32_t xorshift32(
    uint32_t *state)
{
    uint32_t x =
        *state;

    if (x == 0)
    {
        x =
            0xA341316Cu;
    }

    x ^=
        x << 13;

    x ^=
        x >> 17;

    x ^=
        x << 5;

    *state =
        x;

    return x;
}

static unsigned char immediate_mask(
    uint32_t seed,
    size_t position)
{
    uint32_t x =
        seed ^
        0x9E3779B9u ^
        ((uint32_t)position *
         0x85EBCA6Bu);

    x ^=
        x >> 16;

    x *=
        0x7FEB352Du;

    x ^=
        x >> 15;

    x *=
        0x846CA68Bu;

    x ^=
        x >> 16;

    return
        (unsigned char)(
            x & 0xFFu
        );
}

static void build_opcode_map(
    uint32_t seed,
    VMOpcodeMap *map)
{
    unsigned char pool[256];

    for (int i = 0;
         i < 256;
         i++)
    {
        pool[i] =
            (unsigned char)i;

        map->decoded[i] =
            0xFF;
    }

    uint32_t rng =
        seed ^
        0x243F6A88u;

    if (rng == 0)
    {
        rng =
            0x9E3779B9u;
    }

    for (int i = 255;
         i > 0;
         i--)
    {
        uint32_t value =
            xorshift32(
                &rng
            );

        int j =
            (int)(
                value %
                (uint32_t)(i + 1)
            );

        unsigned char temp =
            pool[i];

        pool[i] =
            pool[j];

        pool[j] =
            temp;
    }

    for (int i = 0;
         i < VM_OPCODE_COUNT;
         i++)
    {
        map->encoded[i] =
            pool[i];

        map->decoded[
            pool[i]
        ] =
            (unsigned char)i;
    }
}

static void build_register_map(
    uint32_t seed,
    VMRegisterMap *map)
{
    unsigned char pool[VM_REG_COUNT];

    for (int i = 0;
         i < VM_REG_COUNT;
         i++)
    {
        pool[i] =
            (unsigned char)i;
    }

    uint32_t rng =
        seed ^
        0xC3A5C85Cu;

    if (rng == 0)
    {
        rng =
            0xB5297A4Du;
    }

    for (int i = VM_REG_COUNT - 1;
         i > 0;
         i--)
    {
        uint32_t value =
            xorshift32(
                &rng
            );

        int j =
            (int)(
                value %
                (uint32_t)(i + 1)
            );

        unsigned char temp =
            pool[i];

        pool[i] =
            pool[j];

        pool[j] =
            temp;
    }

    for (int i = 0;
         i < VM_REG_COUNT;
         i++)
    {
        map->encoded[i] =
            pool[i];
    }
}

static void build_order(
    uint32_t seed,
    uint32_t salt,
    unsigned char order[VM_REG_COUNT])
{
    for (int i = 0;
         i < VM_REG_COUNT;
         i++)
    {
        order[i] =
            (unsigned char)i;
    }

    uint32_t rng =
        seed ^
        salt;

    if (rng == 0)
    {
        rng =
            salt |
            1u;
    }

    for (int i = VM_REG_COUNT - 1;
         i > 0;
         i--)
    {
        uint32_t value =
            xorshift32(
                &rng
            );

        int j =
            (int)(
                value %
                (uint32_t)(i + 1)
            );

        unsigned char temp =
            order[i];

        order[i] =
            order[j];

        order[j] =
            temp;
    }
}

static int program_emit8(
    VMProgram *program,
    unsigned char value)
{
    if (program->size >=
        VM_PROGRAM_MAX)
    {
        return 0;
    }

    program->data[
        program->size++
    ] =
        value;

    return 1;
}

static int program_emit16(
    VMProgram *program,
    uint16_t value)
{
    return
        program_emit8(
            program,
            (unsigned char)(
                value &
                0xFFu
            )
        ) &&
        program_emit8(
            program,
            (unsigned char)(
                (value >> 8) &
                0xFFu
            )
        );
}

static int program_emit3(
    VMProgram *program,
    unsigned char opcode,
    unsigned char a,
    unsigned char b)
{
    return
        program_emit8(
            program,
            opcode
        ) &&
        program_emit8(
            program,
            a
        ) &&
        program_emit8(
            program,
            b
        );
}

static int program_emit3_masked(
    VMProgram *program,
    unsigned char opcode,
    unsigned char a,
    unsigned char value,
    uint32_t seed)
{
    size_t immediate_position =
        program->size + 2;

    unsigned char mask =
        immediate_mask(
            seed,
            immediate_position
        );

    unsigned char encoded =
        value ^
        mask;

    return
        program_emit3(
            program,
            opcode,
            a,
            encoded
        );
}

static int program_emit_jnz(
    VMProgram *program,
    const VMOpcodeMap *opcode_map)
{
    if (program->patch_count >=
        VM_MAX_PATCHES)
    {
        return 0;
    }

    if (!program_emit8(
            program,
            opcode_map->encoded[
                LOGICAL_JNZ
            ]
        ))
    {
        return 0;
    }

    program->patches[
        program->patch_count++
    ] =
        program->size;

    return
        program_emit16(
            program,
            0
        );
}

static int program_emit_noise(
    VMProgram *program,
    const VMOpcodeMap *opcode_map,
    uint32_t *rng,
    unsigned int maximum)
{
    unsigned int count =
        xorshift32(
            rng
        ) %
        (maximum + 1);

    for (unsigned int i = 0;
         i < count;
         i++)
    {
        unsigned char a =
            (unsigned char)(
                xorshift32(rng) &
                0xFFu
            );

        unsigned char b =
            (unsigned char)(
                xorshift32(rng) &
                0xFFu
            );

        if (!program_emit3(
                program,
                opcode_map->encoded[
                    LOGICAL_NOP
                ],
                a,
                b
            ))
        {
            return 0;
        }
    }

    return 1;
}

static int program_patch_failures(
    VMProgram *program,
    uint16_t target)
{
    for (size_t i = 0;
         i < program->patch_count;
         i++)
    {
        size_t offset =
            program->patches[i];

        if (offset + 1 >=
            program->size)
        {
            return 0;
        }

        program->data[offset] =
            (unsigned char)(
                target &
                0xFFu
            );

        program->data[
            offset + 1
        ] =
            (unsigned char)(
                (target >> 8) &
                0xFFu
            );
    }

    return 1;
}

static int emit_input_stage(
    VMProgram *program,
    const VMOpcodeMap *opcode_map,
    const VMRegisterMap *register_map,
    unsigned char logical_register,
    uint32_t seed)
{
    unsigned char physical =
        register_map->encoded[
            logical_register
        ];

    switch (logical_register)
    {
        case 0:
        {
            if (!program_emit3(
                    program,
                    opcode_map->encoded[
                        LOGICAL_LOAD_INPUT
                    ],
                    physical,
                    0
                ))
            {
                return 0;
            }

            if (!program_emit3(
                    program,
                    opcode_map->encoded[
                        LOGICAL_XOR_INPUT
                    ],
                    physical,
                    4
                ))
            {
                return 0;
            }

            break;
        }

        case 1:
        {
            if (!program_emit3(
                    program,
                    opcode_map->encoded[
                        LOGICAL_LOAD_INPUT
                    ],
                    physical,
                    1
                ))
            {
                return 0;
            }

            if (!program_emit3(
                    program,
                    opcode_map->encoded[
                        LOGICAL_ADD_INPUT
                    ],
                    physical,
                    5
                ))
            {
                return 0;
            }

            break;
        }

        case 2:
        {
            if (!program_emit3(
                    program,
                    opcode_map->encoded[
                        LOGICAL_LOAD_INPUT
                    ],
                    physical,
                    2
                ))
            {
                return 0;
            }

            if (!program_emit3(
                    program,
                    opcode_map->encoded[
                        LOGICAL_XOR_INPUT
                    ],
                    physical,
                    6
                ))
            {
                return 0;
            }

            if (!program_emit3_masked(
                    program,
                    opcode_map->encoded[
                        LOGICAL_ROL
                    ],
                    physical,
                    3,
                    seed
                ))
            {
                return 0;
            }

            break;
        }

        case 3:
        {
            if (!program_emit3(
                    program,
                    opcode_map->encoded[
                        LOGICAL_LOAD_INPUT
                    ],
                    physical,
                    3
                ))
            {
                return 0;
            }

            if (!program_emit3(
                    program,
                    opcode_map->encoded[
                        LOGICAL_ADD_INPUT
                    ],
                    physical,
                    7
                ))
            {
                return 0;
            }

            break;
        }

        default:
        {
            return 0;
        }
    }

    return 1;
}


static int emit_second_input_stage(
    VMProgram *program,
    const VMOpcodeMap *opcode_map,
    const VMRegisterMap *register_map,
    unsigned char logical_register,
    uint32_t seed)
{
    unsigned char physical =
        register_map->encoded[
            logical_register
        ];

    if (!program_emit3(
            program,
            opcode_map->encoded[
                LOGICAL_LOAD_INPUT
            ],
            physical,
            logical_register
        ))
    {
        return 0;
    }

    if (!program_emit3_masked(
            program,
            opcode_map->encoded[
                LOGICAL_ROL
            ],
            physical,
            1,
            seed
        ))
    {
        return 0;
    }


    if (!program_emit3(
            program,
            opcode_map->encoded[
                LOGICAL_ADD_INPUT
            ],
            physical,
            (unsigned char)(
                logical_register + 4
            )
        ))
    {
        return 0;
    }


    return 1;
}

static int emit_final_transform(
    VMProgram *program,
    const VMOpcodeMap *opcode_map,
    const VMRegisterMap *register_map,
    unsigned char logical_register,
    uint32_t seed)
{
    unsigned char physical =
        register_map->encoded[
            logical_register
        ];

    switch (logical_register)
    {
        case 0:
        {
            return
                program_emit3_masked(
                    program,
                    opcode_map->encoded[
                        LOGICAL_XOR_IMM
                    ],
                    physical,
                    0xA7,
                    seed
                );
        }

        case 1:
        {
            return
                program_emit3_masked(
                    program,
                    opcode_map->encoded[
                        LOGICAL_ADD_IMM
                    ],
                    physical,
                    0x3D,
                    seed
                );
        }

        case 2:
        {
            return
                program_emit3_masked(
                    program,
                    opcode_map->encoded[
                        LOGICAL_ROL
                    ],
                    physical,
                    5,
                    seed
                );
        }

        case 3:
        {
            return
                program_emit3_masked(
                    program,
                    opcode_map->encoded[
                        LOGICAL_XOR_IMM
                    ],
                    physical,
                    0x5C,
                    seed
                );
        }

        default:
        {
            return 0;
        }
    }
}

static int emit_second_final_transform(
    VMProgram *program,
    const VMOpcodeMap *opcode_map,
    const VMRegisterMap *register_map,
    unsigned char logical_register,
    uint32_t seed)
{
    unsigned char physical =
        register_map->encoded[
            logical_register
        ];

    switch (logical_register)
    {
        case 0:
        {
            return
                program_emit3_masked(
                    program,
                    opcode_map->encoded[
                        LOGICAL_XOR_IMM
                    ],
                    physical,
                    0x6D,
                    seed
                );
        }

        case 1:
        {
            return
                program_emit3_masked(
                    program,
                    opcode_map->encoded[
                        LOGICAL_ROL
                    ],
                    physical,
                    3,
                    seed
                );
        }

        case 2:
        {
            return
                program_emit3_masked(
                    program,
                    opcode_map->encoded[
                        LOGICAL_ADD_IMM
                    ],
                    physical,
                    0x47,
                    seed
                );
        }


        case 3:
        {
            return
                program_emit3_masked(
                    program,
                    opcode_map->encoded[
                        LOGICAL_XOR_IMM
                    ],
                    physical,
                    0xB4,
                    seed
                );
        }

        default:
        {
            return 0;
        }
    }
}

static int emit_comparison(
    VMProgram *program,
    const VMOpcodeMap *opcode_map,
    const VMRegisterMap *register_map,
    unsigned char logical_register,
    const unsigned char targets[
        VM_REG_COUNT
    ],
    uint32_t seed)
{
    unsigned char physical =
        register_map->encoded[
            logical_register
        ];

    if (!program_emit3_masked(
            program,
            opcode_map->encoded[
                LOGICAL_CMP
            ],
            physical,
            targets[
                logical_register
            ],
            seed
        ))
    {
        return 0;
    }

    return
        program_emit_jnz(
            program,
            opcode_map
        );
}

static int build_vm_program(
    VMProgram *program,
    const VMOpcodeMap *opcode_map,
    const VMRegisterMap *register_map,
    uint32_t seed)
{
    memset(
        program,
        0,
        sizeof(*program)
    );

    unsigned char input_order[
        VM_REG_COUNT
    ];

    unsigned char final_order[
        VM_REG_COUNT
    ];

    unsigned char compare_order[
        VM_REG_COUNT
    ];

    build_order(
        seed,
        0x13198A2Eu,
        input_order
    );

    build_order(
        seed,
        0x03707344u,
        final_order
    );

    build_order(
        seed,
        0xA4093822u,
        compare_order
    );

    uint32_t noise_rng =
        seed ^
        0x299F31D0u;

    for (int i = 0;
         i < VM_REG_COUNT;
         i++)
    {
        if (!program_emit_noise(
                program,
                opcode_map,
                &noise_rng,
                2
            ))
        {
            return 0;
        }
        if (!emit_input_stage(
                program,
                opcode_map,
                register_map,
                input_order[i],
                seed
            ))
        {
            return 0;
        }
    }

    if (!program_emit_noise(
            program,
            opcode_map,
            &noise_rng,
            3
        ))
    {
        return 0;
    }

    unsigned char r0 =
        register_map->encoded[0];

    unsigned char r1 =
        register_map->encoded[1];

    unsigned char r2 =
        register_map->encoded[2];

    unsigned char r3 =
        register_map->encoded[3];

    if (!program_emit3(
            program,
            opcode_map->encoded[
                LOGICAL_ADD_REG
            ],
            r0,
            r1
        ))
    {
        return 0;
    }


    if (!program_emit_noise(
            program,
            opcode_map,
            &noise_rng,
            1
        ))
    {
        return 0;
    }


    if (!program_emit3(
            program,
            opcode_map->encoded[
                LOGICAL_XOR_REG
            ],
            r1,
            r2
        ))
    {
        return 0;
    }


    if (!program_emit_noise(
            program,
            opcode_map,
            &noise_rng,
            2
        ))
    {
        return 0;
    }


    if (!program_emit3(
            program,
            opcode_map->encoded[
                LOGICAL_ADD_REG
            ],
            r2,
            r3
        ))
    {
        return 0;
    }


    if (!program_emit_noise(
            program,
            opcode_map,
            &noise_rng,
            1
        ))
    {
        return 0;
    }


    if (!program_emit3(
            program,
            opcode_map->encoded[
                LOGICAL_XOR_REG
            ],
            r3,
            r0
        ))
    {
        return 0;
    }


    for (int i = 0;
         i < VM_REG_COUNT;
         i++)
    {
        if (!program_emit_noise(
                program,
                opcode_map,
                &noise_rng,
                2
            ))
        {
            return 0;
        }

        if (!emit_final_transform(
                program,
                opcode_map,
                register_map,
                final_order[i],
                seed
            ))
        {
            return 0;
        }
    }


    if (!program_emit_noise(
            program,
            opcode_map,
            &noise_rng,
            3
        ))
    {
        return 0;
    }

    static const unsigned char
    round1_targets[VM_REG_COUNT] =
    {
        0x82,
        0x7D,
        0x21,
        0xF0
    };


    for (int i = 0;
         i < VM_REG_COUNT;
         i++)
    {
        if (!emit_comparison(
                program,
                opcode_map,
                register_map,
                compare_order[i],
                round1_targets,
                seed
            ))
        {
            return 0;
        }


        if (!program_emit_noise(
                program,
                opcode_map,
                &noise_rng,
                2
            ))
        {
            return 0;
        }
    }

    for (int i = 0;
         i < VM_REG_COUNT;
         i++)
    {
        if (!program_emit_noise(
                program,
                opcode_map,
                &noise_rng,
                2
            ))
        {
            return 0;
        }


        if (!emit_second_input_stage(
                program,
                opcode_map,
                register_map,
                input_order[i],
                seed
            ))
        {
            return 0;
        }
    }

    if (!program_emit3(
            program,
            opcode_map->encoded[
                LOGICAL_XOR_REG
            ],
            r0,
            r1
        ))
    {
        return 0;
    }

    if (!program_emit_noise(
            program,
            opcode_map,
            &noise_rng,
            2
        ))
    {
        return 0;
    }

    if (!program_emit3(
            program,
            opcode_map->encoded[
                LOGICAL_ADD_REG
            ],
            r1,
            r2
        ))
    {
        return 0;
    }

    if (!program_emit_noise(
            program,
            opcode_map,
            &noise_rng,
            1
        ))
    {
        return 0;
    }

    if (!program_emit3(
            program,
            opcode_map->encoded[
                LOGICAL_XOR_REG
            ],
            r2,
            r3
        ))
    {
        return 0;
    }

    if (!program_emit_noise(
            program,
            opcode_map,
            &noise_rng,
            2
        ))
    {
        return 0;
    }

    if (!program_emit3(
            program,
            opcode_map->encoded[
                LOGICAL_ADD_REG
            ],
            r3,
            r0
        ))
    {
        return 0;
    }

    for (int i = 0;
         i < VM_REG_COUNT;
         i++)
    {
        if (!program_emit_noise(
                program,
                opcode_map,
                &noise_rng,
                2
            ))
        {
            return 0;
        }


        if (!emit_second_final_transform(
                program,
                opcode_map,
                register_map,
                final_order[i],
                seed
            ))
        {
            return 0;
        }
    }

    static const unsigned char
    round2_targets[VM_REG_COUNT] =
    {
        0xBF,
        0x32,
        0x0F,
        0x77
    };


    for (int i = 0;
         i < VM_REG_COUNT;
         i++)
    {
        if (!emit_comparison(
                program,
                opcode_map,
                register_map,
                compare_order[i],
                round2_targets,
                seed
            ))
        {
            return 0;
        }

        if (!program_emit_noise(
                program,
                opcode_map,
                &noise_rng,
                2
            ))
        {
            return 0;
        }
    }

    if (!program_emit8(
            program,
            opcode_map->encoded[
                LOGICAL_SUCCESS
            ]
        ))
    {
        return 0;
    }

    if (program->size >
        UINT16_MAX)
    {
        return 0;
    }


    uint16_t failure_address =
        (uint16_t)
        program->size;


    if (!program_emit8(
            program,
            opcode_map->encoded[
                LOGICAL_FAIL
            ]
        ))
    {
        return 0;
    }

    return
        program_patch_failures(
            program,
            failure_address
        );
}

static int vm_read16(
    const unsigned char *program,
    size_t program_size,
    size_t *pc,
    uint16_t *value)
{
    if (*pc + 1 >=
        program_size)
    {
        return 0;
    }

    uint16_t low =
        program[
            (*pc)++
        ];

    uint16_t high =
        program[
            (*pc)++
        ];

    *value =
        low |
        (uint16_t)(
            high << 8
        );

    return 1;
}

static int vm_read_operands(
    const unsigned char *program,
    size_t program_size,
    size_t *pc,
    unsigned char *a,
    unsigned char *b)
{
    if (*pc + 1 >=
        program_size)
    {
        return 0;
    }

    *a =
        program[
            (*pc)++
        ];

    *b =
        program[
            (*pc)++
        ];

    return 1;
}

static int vm_read_masked_operands(
    const unsigned char *program,
    size_t program_size,
    size_t *pc,
    unsigned char *a,
    unsigned char *b,
    uint32_t seed)
{
    if (*pc + 1 >=
        program_size)
    {
        return 0;
    }

    *a =
        program[
            (*pc)++
        ];

    size_t immediate_position =
        *pc;

    unsigned char encoded =
        program[
            (*pc)++
        ];

    *b =
        encoded ^
        immediate_mask(
            seed,
            immediate_position
        );

    return 1;
}

static int vm_run(
    Challenge2VM *vm,
    const unsigned char *program,
    size_t program_size,
    const VMOpcodeMap *opcode_map,
    uint32_t seed)
{
    size_t input_length =
        strlen(
            vm->input
        );

    while (!vm->halted)
    {
        if (vm->pc >=
            program_size)
        {
            return 0;
        }

        unsigned char encoded_opcode =
            program[
                vm->pc++
            ];

        unsigned char opcode =
            opcode_map->decoded[
                encoded_opcode
            ];

        if (opcode >=
            VM_OPCODE_COUNT)
        {
            return 0;
        }

        unsigned char a;
        unsigned char b;

        switch (opcode)
        {
            case LOGICAL_LOAD_INPUT:
            {
                if (!vm_read_operands(
                        program,
                        program_size,
                        &vm->pc,
                        &a,
                        &b
                    ))
                {
                    return 0;
                }

                if (a >=
                        VM_REG_COUNT ||
                    b >=
                        input_length)
                {
                    return 0;
                }

                vm->regs[a] =
                    (unsigned char)
                    vm->input[b];

                break;
            }
            case LOGICAL_XOR_INPUT:
            {
                if (!vm_read_operands(
                        program,
                        program_size,
                        &vm->pc,
                        &a,
                        &b
                    ))
                {
                    return 0;
                }

                if (a >=
                        VM_REG_COUNT ||
                    b >=
                        input_length)
                {
                    return 0;
                }

                vm->regs[a] ^=
                    (unsigned char)
                    vm->input[b];

                vm->regs[a] &=
                    0xFFu;

                break;
            }
            case LOGICAL_ADD_INPUT:
            {
                if (!vm_read_operands(
                        program,
                        program_size,
                        &vm->pc,
                        &a,
                        &b
                    ))
                {
                    return 0;
                }

                if (a >=
                        VM_REG_COUNT ||
                    b >=
                        input_length)
                {
                    return 0;
                }

                vm->regs[a] =
                    (
                        vm->regs[a] +
                        (unsigned char)
                        vm->input[b]
                    ) &
                    0xFFu;

                break;
            }
            case LOGICAL_XOR_REG:
            {
                if (!vm_read_operands(
                        program,
                        program_size,
                        &vm->pc,
                        &a,
                        &b
                    ))
                {
                    return 0;
                }

                if (a >=
                        VM_REG_COUNT ||
                    b >=
                        VM_REG_COUNT)
                {
                    return 0;
                }

                vm->regs[a] ^=
                    vm->regs[b];

                vm->regs[a] &=
                    0xFFu;

                break;
            }
            case LOGICAL_ADD_REG:
            {
                if (!vm_read_operands(
                        program,
                        program_size,
                        &vm->pc,
                        &a,
                        &b
                    ))
                {
                    return 0;
                }

                if (a >=
                        VM_REG_COUNT ||
                    b >=
                        VM_REG_COUNT)
                {
                    return 0;
                }

                vm->regs[a] =
                    (
                        vm->regs[a] +
                        vm->regs[b]
                    ) &
                    0xFFu;

                break;
            }
            case LOGICAL_XOR_IMM:
            {
                if (!vm_read_masked_operands(
                        program,
                        program_size,
                        &vm->pc,
                        &a,
                        &b,
                        seed
                    ))
                {
                    return 0;
                }

                if (a >=
                    VM_REG_COUNT)
                {
                    return 0;
                }

                vm->regs[a] ^=
                    b;

                vm->regs[a] &=
                    0xFFu;

                break;
            }
            case LOGICAL_ADD_IMM:
            {
                if (!vm_read_masked_operands(
                        program,
                        program_size,
                        &vm->pc,
                        &a,
                        &b,
                        seed
                    ))
                {
                    return 0;
                }

                if (a >=
                    VM_REG_COUNT)
                {
                    return 0;
                }

                vm->regs[a] =
                    (
                        vm->regs[a] +
                        b
                    ) &
                    0xFFu;

                break;
            }
            case LOGICAL_ROL:
            {
                if (!vm_read_masked_operands(
                        program,
                        program_size,
                        &vm->pc,
                        &a,
                        &b,
                        seed
                    ))
                {
                    return 0;
                }

                if (a >=
                    VM_REG_COUNT)
                {
                    return 0;
                }

                vm->regs[a] =
                    rol8(
                        (unsigned char)
                        vm->regs[a],
                        b
                    );

                break;
            }
            case LOGICAL_CMP:
            {
                if (!vm_read_masked_operands(
                        program,
                        program_size,
                        &vm->pc,
                        &a,
                        &b,
                        seed
                    ))
                {
                    return 0;
                }

                if (a >=
                    VM_REG_COUNT)
                {
                    return 0;
                }

                vm->zero =
                    (
                        (
                            vm->regs[a] &
                            0xFFu
                        ) ==
                        b
                    );

                break;
            }
            case LOGICAL_JNZ:
            {
                uint16_t target;

                if (!vm_read16(
                        program,
                        program_size,
                        &vm->pc,
                        &target
                    ))
                {
                    return 0;
                }

                if (!vm->zero)
                {
                    if (target >=
                        program_size)
                    {
                        return 0;
                    }

                    vm->pc =
                        target;
                }

                break;
            }
            case LOGICAL_NOP:
            {
                if (!vm_read_operands(
                        program,
                        program_size,
                        &vm->pc,
                        &a,
                        &b
                    ))
                {
                    return 0;
                }

                break;
            }
            case LOGICAL_SUCCESS:
            {
                vm->success =
                    1;

                vm->halted =
                    1;

                break;
            }
            case LOGICAL_FAIL:
            {
                vm->success =
                    0;

                vm->halted =
                    1;

                break;
            }
            default:
            {
                return 0;
            }
        }
    }

    return
        vm->success;
}

static void extract_logical_state(
    const Challenge2VM *vm,
    const VMRegisterMap *register_map,
    unsigned char logical[
        VM_REG_COUNT
    ])
{
    for (int i = 0;
         i < VM_REG_COUNT;
         i++)
    {
        logical[i] =
            (unsigned char)(
                vm->regs[
                    register_map->encoded[i]
                ] &
                0xFFu
            );
    }
}

static void derive_reward_key(
    const unsigned char logical[
        VM_REG_COUNT
    ],
    unsigned char key[
        CHALLENGE2_REWARD_KEY_SIZE
    ])
{
    uint32_t state =
        0x6D2B79F5u;

    state ^=
        (uint32_t)logical[0] *
        0x045D9F3Bu;

    state =
        rotl32(
            state,
            7
        );

    state ^=
        (uint32_t)logical[1] *
        0x119DE1F3u;

    state =
        rotl32(
            state,
            11
        );

    state ^=
        (uint32_t)logical[2] *
        0x3449F515u;

    state =
        rotl32(
            state,
            13
        );

    state ^=
        (uint32_t)logical[3] *
        0x27D4EB2Du;

    for (size_t i = 0;
         i <
         CHALLENGE2_REWARD_KEY_SIZE;
         i++)
    {
        uint32_t value =
            xorshift32(
                &state
            );

        key[i] =
            (unsigned char)(
                value ^
                (value >> 8) ^
                (value >> 16) ^
                (value >> 24) ^
                ((uint32_t)i *
                 0x5Bu)
            );
    }
}



void challenge2_init(
    Challenge2State *state)
{
    if (state == NULL)
    {
        return;
    }

    memset(
        state,
        0,
        sizeof(*state)
    );

    state->seed =
        0x504F4C59u;
}

void challenge2_record_move(
    Challenge2State *state,
    int from,
    int to)
{
    if (state == NULL)
    {
        return;
    }

    if (state->move_count >=
        CHALLENGE2_MOVE_COUNT)
    {
        return;
    }

    uint16_t encoded_move =
        ((uint16_t)(
            from &
            0xFF
        ) << 8) |
        (uint16_t)(
            to &
            0xFF
        );

    state->moves[
        state->move_count
    ] =
        encoded_move;

    state->move_count++;
}

int challenge2_is_ready(
    const Challenge2State *state)
{
    if (state == NULL)
    {
        return 0;
    }

    return
        state->move_count >=
        CHALLENGE2_MOVE_COUNT;
}

uint32_t challenge2_build_seed(
    const Challenge2State *state)
{
    if (state == NULL)
    {
        return 0;
    }

    uint32_t seed =
        0x504F4C59u;

    for (size_t i = 0;
         i <
         state->move_count;
         i++)
    {
        seed ^=
            (uint32_t)
            state->moves[i];

        seed =
            rotl32(
                seed,
                5
            );

        seed ^=
            0x9E3779B9u +
            (uint32_t)i;

        seed *=
            0x45D9F3Bu;
    }

    return seed;
}

int challenge2_check_password(
    Challenge2State *state,
    const char *password)
{
    if (state == NULL ||
        password == NULL)
    {
        return 0;
    }

    if (antidebug_detected())
    {
        return 0;
    }

    state->reward_ready =
        0;

    memset(
        state->reward_key,
        0,
        sizeof(
            state->reward_key
        )
    );

    state->reward_key_size =
        0;

    if (strlen(password) !=
        8 || !password_charset_valid(
        password
    ))
    {
        return 0;
    }

    state->seed =
        challenge2_build_seed(
            state
        );

    VMOpcodeMap opcode_map;

    build_opcode_map(
        state->seed,
        &opcode_map
    );

    VMRegisterMap register_map;

    build_register_map(
        state->seed,
        &register_map
    );

    VMProgram program;

    if (!build_vm_program(
            &program,
            &opcode_map,
            &register_map,
            state->seed
        ))
    {
        return 0;
    }

    Challenge2VM vm;

    memset(
        &vm,
        0,
        sizeof(vm)
    );

    vm.input =
        password;

    int result =
        vm_run(
            &vm,
            program.data,
            program.size,
            &opcode_map,
            state->seed
        );

    if (!result)
    {
        return 0;
    }

    unsigned char logical[
        VM_REG_COUNT
    ];

    extract_logical_state(
        &vm,
        &register_map,
        logical
    );

    derive_reward_key(
        logical,
        state->reward_key
    );

    state->reward_key_size =
        CHALLENGE2_REWARD_KEY_SIZE;

    state->reward_ready =
        1;

    return 1;
}

void challenge2_print_reward(
    const Challenge2State *state)
{
    if (state == NULL)
    {
        return;
    }

    if (!state->reward_ready ||
        state->reward_key_size <
            REWARD_SIZE)
    {
        printf(
            "\nReward state invalid.\n"
        );

        return;
    }

    unsigned char output[
        REWARD_SIZE
    ];

    for (size_t i = 0;
         i < REWARD_SIZE;
         i++)
    {
        output[i] =
            encrypted_reward[i] ^
            state->reward_key[i];
    }

    printf(
        "\n"
        "========================================\n"
        "          CHALLENGE 2 SOLVED\n"
        "========================================\n"
        "\n"
    );

    fwrite(
        output,
        1,
        REWARD_SIZE,
        stdout
    );

    printf(
        "\n\n"
    );
}