#ifndef __attribute_h__
#define __attribute_h__
#include "multisys.h"
#pragma pack(push, 1)
namespace dc {
    enum {
        type_s64,
        type_string,

        type_can_not_be_key,
        type_float = type_can_not_be_key,
        type_bool,
    };

    struct layout {
        const std::string _name; 
        const s16 _index;
        const s32 _offset;
        const s8 _type;
        const s16 _size;
        const bool _visual; 
        const bool _share;
        const bool _save;
        const bool _important;

        layout(const char * name, s16 index, s32 offset, s8 type, s16 size, bool visual, bool share, bool save, bool important) : _name(name), _index(index), _offset(offset), _type(type), _size(size), _visual(visual), _share(share), _save(save), _important(important) {}
    };

    static const char * files[] = {"base","biology","buff","bufftarget","combiner","effect","fsm","orbit","player","role","scene","sceneeffect","skill"};
    namespace base {
        static const char * name = "base";
        namespace attr {
        }

        namespace table {
        }
    }
    namespace biology {
        static const char * name = "biology";
        namespace attr {
               static const layout state("state", 0, 0, type_s64, 8, true, true, false, false);
               static const layout y("y", 1, 8, type_float, 4, true, true, false, false);
               static const layout speed("speed", 2, 12, type_s64, 8, true, true, false, false);
        }

        namespace table {
        }
    }
    namespace buff {
        static const char * name = "buff";
        namespace attr {
               static const layout scene("scene", 0, 0, type_s64, 8, false, false, false, false);
               static const layout configid("configid", 1, 8, type_s64, 8, false, false, false, false);
               static const layout type("type", 2, 16, type_s64, 8, false, false, false, false);
               static const layout attr_type("attr_type", 3, 24, type_s64, 8, true, true, false, false);
               static const layout delay("delay", 4, 32, type_s64, 8, true, true, false, false);
               static const layout interval("interval", 5, 40, type_s64, 8, true, true, false, false);
               static const layout count("count", 6, 48, type_s64, 8, false, false, false, false);
               static const layout value("value", 7, 56, type_float, 4, true, true, false, false);
        }

        namespace table {
            namespace target {
                static const char * name = "target";
                static const s32 tag = 0;
                enum {
                    column_id_int64_key,
                    column_value_float,
                };
            }
        }
    }
    namespace bufftarget {
        static const char * name = "bufftarget";
        namespace attr {
               static const layout state("state", 0, 0, type_s64, 8, true, true, false, false);
               static const layout y("y", 1, 8, type_float, 4, true, true, false, false);
               static const layout speed("speed", 2, 12, type_s64, 8, true, true, false, false);
               static const layout weight("weight", 3, 20, type_float, 4, true, true, false, false);
               static const layout discollision("discollision", 4, 24, type_s64, 8, true, true, false, false);
               static const layout invisible("invisible", 5, 32, type_s64, 8, true, true, false, false);
               static const layout congelation("congelation", 6, 40, type_s64, 8, true, true, false, false);
        }

        namespace table {
            namespace buff {
                static const char * name = "buff";
                static const s32 tag = 0;
                enum {
                    column_id_int64_key,
                };
            }
        }
    }
    namespace combiner {
        static const char * name = "combiner";
        namespace attr {
               static const layout state("state", 0, 0, type_s64, 8, true, true, false, false);
               static const layout y("y", 1, 8, type_float, 4, true, true, false, false);
               static const layout speed("speed", 2, 12, type_s64, 8, true, true, false, false);
               static const layout weight("weight", 3, 20, type_float, 4, true, true, false, false);
               static const layout discollision("discollision", 4, 24, type_s64, 8, true, true, false, false);
               static const layout invisible("invisible", 5, 32, type_s64, 8, true, true, false, false);
               static const layout congelation("congelation", 6, 40, type_s64, 8, true, true, false, false);
               static const layout scene("scene", 7, 48, type_s64, 8, true, true, false, false);
               static const layout orbit("orbit", 8, 56, type_s64, 8, true, true, false, false);
               static const layout top_size("top_size", 9, 64, type_float, 4, true, true, false, false);
               static const layout bottom_size("bottom_size", 10, 68, type_float, 4, true, true, false, false);
               static const layout top_weight("top_weight", 11, 72, type_float, 4, true, true, false, false);
               static const layout bottom_weight("bottom_weight", 12, 76, type_float, 4, true, true, false, false);
        }

        namespace table {
            namespace buff {
                static const char * name = "buff";
                static const s32 tag = 0;
                enum {
                    column_id_int64_key,
                };
            }
            namespace top_role {
                static const char * name = "top_role";
                static const s32 tag = 1;
                enum {
                    column_id_int64_key,
                };
            }
            namespace bottom_role {
                static const char * name = "bottom_role";
                static const s32 tag = 2;
                enum {
                    column_id_int64_key,
                };
            }
        }
    }
    namespace effect {
        static const char * name = "effect";
        namespace attr {
               static const layout configid("configid", 0, 0, type_s64, 8, false, false, false, false);
               static const layout type("type", 1, 8, type_s64, 8, false, false, false, false);
               static const layout time("time", 2, 16, type_s64, 8, false, false, false, false);
               static const layout scene("scene", 3, 24, type_s64, 8, false, false, false, false);
               static const layout orbit("orbit", 4, 32, type_s64, 8, false, false, false, false);
        }

        namespace table {
        }
    }
    namespace fsm {
        static const char * name = "fsm";
        namespace attr {
               static const layout state("state", 0, 0, type_s64, 8, true, true, false, false);
        }

        namespace table {
        }
    }
    namespace orbit {
        static const char * name = "orbit";
        namespace attr {
               static const layout close("close", 0, 0, type_s64, 8, false, false, false, false);
               static const layout scene("scene", 1, 8, type_s64, 8, false, false, false, false);
               static const layout number("number", 2, 16, type_s64, 8, false, false, false, false);
               static const layout length("length", 3, 24, type_float, 4, false, false, false, false);
               static const layout combiner("combiner", 4, 28, type_s64, 8, false, false, false, false);
        }

        namespace table {
            namespace top {
                static const char * name = "top";
                static const s32 tag = 0;
                enum {
                    column_id_int64_key,
                };
            }
            namespace bottom {
                static const char * name = "bottom";
                static const s32 tag = 1;
                enum {
                    column_id_int64_key,
                };
            }
        }
    }
    namespace player {
        static const char * name = "player";
        namespace attr {
               static const layout state("state", 0, 0, type_s64, 8, true, true, false, false);
               static const layout gate("gate", 1, 8, type_s64, 8, false, false, false, false);
               static const layout match("match", 2, 16, type_s64, 8, false, false, false, false);
               static const layout name("name", 3, 24, type_string, 512, true, true, false, false);
               static const layout icon("icon", 4, 536, type_string, 1024, true, true, false, false);
               static const layout scene("scene", 5, 1560, type_s64, 8, false, false, false, false);
               static const layout side("side", 6, 1568, type_s64, 8, true, true, false, false);
               static const layout isai("isai", 7, 1576, type_bool, 1, true, true, false, false);
               static const layout last_cd_tick("last_cd_tick", 8, 1577, type_s64, 8, true, true, false, false);
        }

        namespace table {
            namespace card_used {
                static const char * name = "card_used";
                static const s32 tag = 0;
                enum {
                    column_config_id_int64_key,
                };
            }
            namespace role_queue {
                static const char * name = "role_queue";
                static const s32 tag = 1;
                enum {
                    column_id_int64,
                };
            }
        }
    }
    namespace role {
        static const char * name = "role";
        namespace attr {
               static const layout state("state", 0, 0, type_s64, 8, true, true, false, false);
               static const layout y("y", 1, 8, type_float, 4, true, true, false, false);
               static const layout speed("speed", 2, 12, type_s64, 8, true, true, false, false);
               static const layout weight("weight", 3, 20, type_float, 4, true, true, false, false);
               static const layout discollision("discollision", 4, 24, type_s64, 8, true, true, false, false);
               static const layout invisible("invisible", 5, 32, type_s64, 8, true, true, false, false);
               static const layout congelation("congelation", 6, 40, type_s64, 8, true, true, false, false);
               static const layout scene("scene", 7, 48, type_s64, 8, false, false, false, false);
               static const layout player("player", 8, 56, type_s64, 8, false, false, false, false);
               static const layout combiner("combiner", 9, 64, type_s64, 8, false, false, false, false);
               static const layout configid("configid", 10, 72, type_s64, 8, true, true, false, false);
               static const layout orbit("orbit", 11, 80, type_s64, 8, true, true, false, false);
               static const layout orbit_num("orbit_num", 12, 88, type_s64, 8, true, true, false, false);
               static const layout point("point", 13, 96, type_s64, 8, false, false, false, false);
               static const layout name("name", 14, 104, type_string, 256, true, true, false, false);
               static const layout skill("skill", 15, 360, type_s64, 8, true, true, false, false);
               static const layout base_weight("base_weight", 16, 368, type_float, 4, true, true, false, false);
               static const layout up_weight("up_weight", 17, 372, type_float, 4, true, true, false, false);
               static const layout side("side", 18, 376, type_s64, 8, true, true, false, false);
               static const layout size("size", 19, 384, type_float, 4, true, true, false, false);
        }

        namespace table {
            namespace buff {
                static const char * name = "buff";
                static const s32 tag = 0;
                enum {
                    column_id_int64_key,
                };
            }
            namespace skill {
                static const char * name = "skill";
                static const s32 tag = 1;
                enum {
                    column_id_int64_key,
                };
            }
            namespace collide_cast {
                static const char * name = "collide_cast";
                static const s32 tag = 2;
                enum {
                    column_skill_id_int64_key,
                };
            }
            namespace encounter_cast {
                static const char * name = "encounter_cast";
                static const s32 tag = 3;
                enum {
                    column_skill_id_int64_key,
                };
            }
        }
    }
    namespace scene {
        static const char * name = "scene";
        namespace attr {
               static const layout close("close", 0, 0, type_s64, 8, false, false, false, false);
               static const layout config("config", 1, 8, type_s64, 8, true, true, false, false);
               static const layout orbit_count("orbit_count", 2, 16, type_s64, 8, true, true, false, false);
               static const layout orbit_length("orbit_length", 3, 24, type_float, 4, true, true, false, false);
               static const layout res("res", 4, 28, type_string, 128, true, true, false, false);
               static const layout start_tick("start_tick", 5, 156, type_s64, 8, false, false, false, false);
               static const layout game_time("game_time", 6, 164, type_s64, 8, false, false, false, false);
        }

        namespace table {
            namespace member {
                static const char * name = "member";
                static const s32 tag = 0;
                enum {
                    column_id_int64_key,
                    column_gate_int64,
                    column_state_int64,
                    column_point_int64,
                };
            }
            namespace orbit {
                static const char * name = "orbit";
                static const s32 tag = 1;
                enum {
                    column_number_int64_key,
                    column_id_int64,
                };
            }
            namespace role {
                static const char * name = "role";
                static const s32 tag = 2;
                enum {
                    column_role_id_int64_key,
                };
            }
            namespace buff {
                static const char * name = "buff";
                static const s32 tag = 3;
                enum {
                    column_id_int64_key,
                };
            }
            namespace effect {
                static const char * name = "effect";
                static const s32 tag = 4;
                enum {
                    column_id_int64_key,
                };
            }
        }
    }
    namespace sceneeffect {
        static const char * name = "sceneeffect";
        namespace attr {
               static const layout close("close", 0, 0, type_s64, 8, false, false, false, false);
        }

        namespace table {
        }
    }
    namespace skill {
        static const char * name = "skill";
        namespace attr {
               static const layout host("host", 0, 0, type_s64, 8, false, false, false, false);
               static const layout configid("configid", 1, 8, type_s64, 8, false, false, false, false);
               static const layout type("type", 2, 16, type_s64, 8, false, false, false, false);
               static const layout superiority_and_inferiority("superiority_and_inferiority", 3, 24, type_s64, 8, true, true, false, false);
               static const layout cast_type("cast_type", 4, 32, type_s64, 8, true, true, false, false);
               static const layout count("count", 5, 40, type_s64, 8, false, false, false, false);
               static const layout value("value", 6, 48, type_string, 256, true, true, false, false);
               static const layout des("des", 7, 304, type_s64, 8, true, true, false, false);
        }

        namespace table {
            namespace buffs {
                static const char * name = "buffs";
                static const s32 tag = 0;
                enum {
                    column_configid_int64_key,
                    column_type_int64,
                    column_life_cycle_int64,
                };
            }
        }
    }

    namespace global {
    }
}
#pragma pack(pop)
#endif //__attribute_h__
