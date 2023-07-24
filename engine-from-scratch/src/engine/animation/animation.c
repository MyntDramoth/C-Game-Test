#include "animation.h"

#include <assert.h>
#include "../util.h"
#include "../array_list/array_list.h"

static Array_List *animation_def_storage;
static Array_List *animation_storage;

void animation_init(void) {
    animation_def_storage = array_list_create(sizeof(Animation_Def),0);
    animation_storage = array_list_create(sizeof(Animation),0);
}

usize animation_def_create(Sprite_Sheet *sprite_sheet,f32 *durations, u8 *rows, u8 *columns, u8 frame_count) {
    assert(frame_count <= MAX_FRAMES);
    Animation_Def def = {0};
    def.sprite_sheet = sprite_sheet;
    def.frame_count = frame_count;

    for(u8 i = 0;i<frame_count;++i) {
        def.frames[i] = (Animation_Frame){
            .column = columns[i],
            .row = rows[i],
            .duration = durations[i],
        };
    }
    return array_list_append(animation_def_storage,&def);
}

usize animation_create(usize animation_def_id, bool does_loop) {
    usize id = animation_storage->len;
    Animation_Def *adef = array_list_get(animation_def_storage, animation_def_id);
    if(adef == NULL) {
        ERROR_EXIT("Animation definition with id: %zu not found", animation_def_id);
    }

    //try to find free slot first
    for(usize i = 0; i < animation_storage->len;++i) {
        Animation *animation = array_list_get(animation_storage,i);
        if(!animation->is_active) {
            id = i;
            break;
        }   
    }

    if(id == animation_storage->len) {
        array_list_append(animation_storage,&(Animation){0});
    }

    Animation *animation = array_list_get(animation_storage,id);

    //other fields default to 0 when using field dot syntax
    *animation = (Animation) {
        .definition = adef,
        .does_loop = does_loop,
        .is_active = true,
    };

    return id;
}

void animation_destroy(usize id) {
     Animation *animation = array_list_get(animation_storage,id);
     animation->is_active = false;
}

Animation* animation_get(usize id) {
    return array_list_get(animation_storage,id);
}

void animation_update(f32 dt) {
    for(usize i = 0;i < animation_storage->len;++i) {
         Animation *animation = array_list_get(animation_storage,i);
         Animation_Def *adef = animation->definition;
         animation->current_frame_time -= dt;
       
         if(animation->current_frame_time <= 0) {
            animation->current_frame_index +=1 ;

            //loop or stay on last frame
            if(animation->current_frame_index == adef->frame_count) {
                if(animation->does_loop) {
                    animation->current_frame_index = 0;
                } else {
                    animation->current_frame_index -= 1;
                }
            }
            animation->current_frame_time = adef->frames[animation->current_frame_index].duration;
        }
    }
}