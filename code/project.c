#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#define LOG_ENABLED
#define bottom screen_H + 720 - 400
#define pi acos(-1)
#define max_enemy 6
#define mob_radius 30
#define mob_bullet_r 35
typedef struct {
    float x;
    float y;
    float vx;
    float vy;
    float radius;
    int mod;
    int time;
    bool hidden;
    ALLEGRO_BITMAP* img;
}enemy_bullet;
typedef struct {
    float x;
    float y;
    int hp;
    int att_cd;
    bool hidden;
    int mod;
    ALLEGRO_BITMAP* img;
    ALLEGRO_BITMAP* img_bullet;
}enemy;
typedef struct {
    float x;
    float y;
    float vx;
    float vy;
    float radius;
    int start;
    bool hidden;
    ALLEGRO_BITMAP* img;
}player_bullet;
typedef struct {
    float boss_x;
    float boss_y;
    float boss_vx;
    float boss_vy;
    float radius;
    int boss_w;
    int boss_h;
    ALLEGRO_BITMAP* boss_att;
    ALLEGRO_BITMAP* boss_stay;
    ALLEGRO_BITMAP* boss_skill1;
    ALLEGRO_BITMAP* boss_skill1_eff[12];
    ALLEGRO_BITMAP* boss_skill2;
    ALLEGRO_BITMAP* boss_skill4[4];
    ALLEGRO_BITMAP* die[6];
    ALLEGRO_BITMAP* bullet_img;
}stage1_b;
typedef struct {
    int lazer_x;
    int width;
    ALLEGRO_BITMAP* lazer_img[10];
    ALLEGRO_BITMAP* lazer_pre[18];
}lazer;
typedef struct mush_s {
    float x;
    float y;
    float vy;
    float radius;
    ALLEGRO_BITMAP* img;
    struct mush_s* next_bullet;
}falling_mush;
typedef struct bullet_s {
    float x;
    float y;
    float vx;
    float vy;
    float radius;
    int lim_x;
    int lim_y;
    ALLEGRO_BITMAP* img;
    struct bullet_s* next_bullet;
}bullet;
// rotate 以圖的哪個點為轉軸 那個點放哪 順時轉(弧度)
//system
const int screen_W = 1280;
const int screen_H = 720;
const int FPS = 60;
int now_time;
int R_bound = 1080;
char str[80];
int rand_seed = 4;
int score;
int die;
int diffi_mode;
ALLEGRO_DISPLAY* display;
ALLEGRO_EVENT event;
ALLEGRO_FONT* font;
int mouse_x, mouse_y;
int camera_x = -(1280 / 2) + 400 + 100 / 2;
int camera_y = -487;
ALLEGRO_SAMPLE* pick;

//player
int act_x = 400;
int act_y = 1050;
player_bullet sword_wave[4];
ALLEGRO_BITMAP* skill_img[3];
ALLEGRO_BITMAP* skill2_img_mov[6];
ALLEGRO_BITMAP* skill2_img_movL[6];
ALLEGRO_BITMAP* farR;
ALLEGRO_BITMAP* farL;
ALLEGRO_BITMAP* farU;
ALLEGRO_BITMAP* skill1_eff[30];
ALLEGRO_BITMAP* skill1_pre[38];
ALLEGRO_BITMAP* skill1_invi[14];
ALLEGRO_BITMAP* skill2_eff_up[17];
ALLEGRO_BITMAP* skill2_eff_down[17];
ALLEGRO_BITMAP* skill2_eff_upR[17];
ALLEGRO_BITMAP* skill2_eff_downR[17];
int player_skill2_ind;
ALLEGRO_BITMAP* skill3_img[28];
ALLEGRO_BITMAP* player_skill1_img[9];
ALLEGRO_BITMAP* player_jump[2];
ALLEGRO_BITMAP* player_dash[2];
ALLEGRO_BITMAP* player_dash_img[2][6];
int player_dash_ind = 0;
ALLEGRO_BITMAP* player_attL[6];
int player_attL_ind = 0;
ALLEGRO_BITMAP* player_attR[6];
int player_attR_ind = 0;
ALLEGRO_BITMAP* att_effectL[4];
int att_effect_indL = 0;
ALLEGRO_BITMAP* att_effectR[4];
int att_effect_indR = 0;
ALLEGRO_BITMAP* playerL[5];
int playerL_ind = 0;
ALLEGRO_BITMAP* playerR[5];
int playerR_ind = 0;
ALLEGRO_BITMAP* player[2];
int player_hp = 6;
int player_mp;
int player_got_dmg_time = -50;
int player_skill_ind = 0;
int invi = 0;
int skill1_af_ind = 0;
int invi_ind;
//player move
float jump_v = 1.7;
float dash_v = 3;
int last_att = -35;
int skill_pre_ind;
int skill_eff_ind;
int skill1_cd = 0;
int skill_ind;


ALLEGRO_BITMAP* back;
ALLEGRO_BITMAP* att;
ALLEGRO_EVENT_QUEUE* event_queue;
ALLEGRO_TIMER* timer;
ALLEGRO_TRANSFORM camera;
ALLEGRO_SAMPLE* BGM;
ALLEGRO_SAMPLE* BGM2;
ALLEGRO_SAMPLE* BGM3;
ALLEGRO_SAMPLE_INSTANCE* bgm_instance;
ALLEGRO_SAMPLE* dash_sound;
ALLEGRO_SAMPLE* player_skill1_sound;
ALLEGRO_SAMPLE* player_skill3_sound;
ALLEGRO_SAMPLE* got_dmg;
ALLEGRO_SAMPLE* flower_sd;
ALLEGRO_SAMPLE* player_skill1_sd_af;
ALLEGRO_SAMPLE* player_skill3_sd_af;
ALLEGRO_SAMPLE* player_skill2_up_sd;
ALLEGRO_SAMPLE* player_skill2_down_sd;

// effect
ALLEGRO_BITMAP* sum[14];
int sum_ind = 0;
ALLEGRO_BITMAP* fader;
ALLEGRO_BITMAP* skill_fader;
ALLEGRO_BITMAP* backup;
ALLEGRO_BITMAP* stop_img;
ALLEGRO_BITMAP* setting_img;
ALLEGRO_BITMAP* choose[3][2];
ALLEGRO_BITMAP* hitted_struck[4];

int bullet_max;
int bullet_num;
int bullet_ct;
int stop_bullet;

//boss
stage1_b st1_boss;
lazer lazer_att[9];
enemy mobs[6];
enemy_bullet mobs_bullet[100];
int boss_att_time = -100;
int boss_att_ind = 0;
float boss_hp = 200;
ALLEGRO_BITMAP* skill6_eff[13];
ALLEGRO_BITMAP* flower;
ALLEGRO_BITMAP* mob_img[3];
ALLEGRO_BITMAP* mob_bullet_img[3];
ALLEGRO_BITMAP* boss2_ult_img[22];
ALLEGRO_BITMAP* boss2_ult_back;
ALLEGRO_BITMAP* boss2_ult_anime[14];
ALLEGRO_BITMAP* boss3_sum[19];
ALLEGRO_BITMAP* boss3_ult1_img;
ALLEGRO_BITMAP* boss3_ult2_img[33];
ALLEGRO_BITMAP* boss3_ult2_black;
int boss3_ult2_ind;
ALLEGRO_SAMPLE* hitted;
ALLEGRO_SAMPLE* hit_buttom;
ALLEGRO_SAMPLE* st1_boss_skill4_sd;
ALLEGRO_SAMPLE* st1_boss_skill2_sd;
ALLEGRO_SAMPLE* st1_boss_skill6_sd;
ALLEGRO_SAMPLE* boss_ult1_sd;
ALLEGRO_SAMPLE* stop_sd;
int skill5_lim;
int boss3_ult1_time;
int hitted_ind;
int skill6_time;
int boss_last_att_ind;

//judge
bool difficulty;
bool skill1;
bool freeze;
int boss_phase;
bool boss_get_dmg;
int last_boss_dmg;
bool flower_stop;
bool boss_ult1;
bool boss_ult2;
bool get_small;
bool can_dash;
int last_dash;
bool dir;
bool can_move;
int show_norm_att;
bool can_up;
bool key_state[ALLEGRO_KEY_MAX];

void game_abort(const char* format, ...);

void game_log(const char* format, ...);

void game_vlog(const char* format, va_list arg);

void allegro5_init();

void game_init();

void game_loop();

void draw_st1_boss();

void player_draw();

void player_move();

void player_skill1();

void camera_update();

void boss1_init();

void boss2_init();

void boss3_init();

void boss4_init();

void game_stage1_sum();

void game_stage1();

void st1_boss_skill1();

void st1_boss_skill2();

void st1_boss_skill3();

void st1_boss_skill4();

void st1_boss_skill5();

void st1_boss_skill6();

void boss2_ult();

void boss3_ult1();

void boss3_ult2();

void st1_to_2();

void st2_to_3();

float jump_up();

float jump_down();

float dash();

void restart_init();

void setting();

void beginning();

void again();

void end();

void go_to_mid();

void go_buttom();

void background();

void drawHP();

void draw_bullet();

float bullet_down(float *);

float distance(float x1, float y1, float x2, float y2);

void flower_mem(bullet* temp);

void mush_memory(falling_mush*);

void circle_mem(bullet*);

void circle_move(bullet*);

void collision(float,float,float);

void st1_make_bullet(falling_mush*);

void flower_bullet(bullet*);

void del_flower(bullet*, bullet*);

void del_bullet(falling_mush*, falling_mush*);

void destroy1();

void destroy2();

void destroy3();

int main() {
    allegro5_init();
    game_log("allegro5 initialized successfully!!");
    game_init();
    game_log("Game initialized successfully!!");
    camera_update();
    beginning();
    return 0;
}

void allegro5_init() {
    if (!al_init())
        game_abort("failed to initialized allegro5");

    if (!al_install_keyboard())
        game_abort("failed to initialized keyboard");
    if (!al_install_mouse())
        game_abort("failed to load mouse");

    event_queue = al_create_event_queue();
    if (!event_queue)
        game_abort("failed to create event queue");

    display = al_create_display(screen_W,screen_H);
    if (!display)
        game_abort("failed to create display");

    timer = al_create_timer(1.0f / FPS);
    if (!timer)
        game_abort("failed to create timer");

    if (!al_init_primitives_addon())
        game_abort("failed to primitive addon");

    if (!al_init_image_addon())
        game_abort("failed to image addon");

    al_init_font_addon();

    if (!al_init_ttf_addon())
        game_abort("failed to ttf addon");

    if (!al_install_audio())
        game_abort("failed to install audio");
    if (!al_init_acodec_addon())
        game_abort("failed to initiate acodec");
    al_reserve_samples(100);

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    //al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue,al_get_mouse_event_source());

    bgm_instance = al_create_sample_instance(BGM);
    if (!bgm_instance)
        game_abort("failed to load bgm_instance");

    al_start_timer(timer);
    srand(time(NULL));
}

void game_init() {
    // player
    player[1] = al_load_bitmap("image/player/stay/STAYR.png");
    if (!player[1])
        game_abort("failed to load STAYR.png");
    player[0] = al_load_bitmap("image/player/stay/STAYL.png");
    if (!player[0])
        game_abort("failed to load STAYL.png");
    player_jump[0] = al_load_bitmap("image/player/jump/jumpL.png");
    if (!player_jump[0])
        game_abort("failed to load jumpL.png");
    player_jump[1] = al_load_bitmap("image/player/jump/jumpR.png");
    if (!player_jump[1])
        game_abort("failed to load jumpR.png");
    player_dash[0] = al_load_bitmap("image/player/dash/dashL.png");
    if (!player_dash[0])
        game_abort("failed to load jumpL.png");
    player_dash[1] = al_load_bitmap("image/player/dash/dashR.png");
    if (!player_dash[1])
        game_abort("failed to load jumpR.png");
    for (int i = 0; i < 6; ++i)
    {
        sprintf(str, "image/player/dash/dash%d.png", i);
        player_dash_img[1][i] = al_load_bitmap(str);
        if (!player_dash_img[1][i])
            game_abort("failed to load dash img%d", i);
        sprintf(str, "image/player/dash/dashL%d.png", i);
        player_dash_img[0][i] = al_load_bitmap(str);
    }
    for (int i = 0; i < 5; ++i)
    {
        sprintf(str, "image/player/right/RIGHT%d.png", i);
        playerR[i] = al_load_bitmap(str);
        if (!playerR[i])
            game_abort("failed to load playerR[%d]", i);
    }
    for (int i = 0; i < 5; ++i)
    {
        sprintf(str, "image/player/left/LEFT%d.png", i);
        playerL[i] = al_load_bitmap(str);
        if (!playerL[i])
            game_abort("failed to load playerL[%d]", i);
    }
    for (int i = 0; i < 6; ++i)
    {
        sprintf(str, "image/player/att/attL%d.png", i);
        player_attL[i] = al_load_bitmap(str);
        if (!player_attL[i])
            game_abort("failed to load attL[%d]", i);
    }
    for (int i = 0; i < 6; ++i)
    {
        sprintf(str, "image/player/att/attR%d.png", i);
        player_attR[i] = al_load_bitmap(str);
        if (!player_attR[i])
            game_abort("failed to load attR[%d]", i);
    }
    for (int i = 0; i < 4; ++i)
    {
        sprintf(str, "image/player/att_effect/att_effectL%d.png", i);
        att_effectL[i] = al_load_bitmap(str);
        if (!att_effectL[i])
            game_abort("failed to load att_effectL[%d]", i);
    }
    for (int i = 0; i < 4; ++i)
    {
        sprintf(str, "image/player/att_effect/att_effectR%d.png", i);
        att_effectR[i] = al_load_bitmap(str);
        if (!att_effectR[i])
            game_abort("failed to load att_effectR[%d]", i);
    }
    for (int i = 0; i < 29; ++i)
    {
        sprintf(str, "image/player/skill1/effect/skill1%d.png", i+1);
        skill1_eff[i] = al_load_bitmap(str);
        if (!skill1_eff[i])
            game_abort("failed to load skill1 eff %d", i);
    }
    for (int i = 0; i < 38; ++i)
    {
        sprintf(str, "image/player/skill1/test%d.png", i);
        skill1_pre[i] = al_load_bitmap(str);
        if (!skill1_pre[i])
            game_abort("failed to load skill1 pre %d", i);
    }
    for (int i = 0; i < 6; ++i)
    {
        sprintf(str, "image/player/skill1/skill%d.png", i+3);
        player_skill1_img[i] = al_load_bitmap(str);
        if (!player_skill1_img[i])
            game_abort("failed to load player skill1 %d", i);
    }
    for (int i = 0; i < 6; ++i) {
        sprintf(str, "image/player/skill2/player_skill2_move_%d.png", i);
        skill2_img_mov[i] = al_load_bitmap(str);
        if (!skill2_img_mov[i])
            game_abort("failed to load skill2 mov %d", i);
    }
    for (int i = 0; i < 6; ++i) {
        sprintf(str, "image/player/skill2/player_skill2_moveL_%d.png", i);
        skill2_img_movL[i] = al_load_bitmap(str);
        if (!skill2_img_movL[i])
            game_abort("failed to load skill2 movL %d", i);
    }
    for (int i = 0; i < 4; ++i)
    {
        sword_wave[i].hidden = 1;
        sword_wave[i].radius = 25;
    }
    skill_img[0] = al_load_bitmap("image/player/skill1/P_skill1_img.png");
    if (!skill_img[0])
        game_abort("failed to load skill1 img");
    skill_img[1] = al_load_bitmap("image/player/skill2/P_skill2_img.png");
    if (!skill_img[1])
        game_abort("failed to load skill2 img");
    skill_img[2] = al_load_bitmap("image/player/skill3/P_skill3_img.png");
    if (!skill_img[2])
        game_abort("failed to load skill3 img");
    farR = al_load_bitmap("image/player/att_far/farR.png");
    if (!farR)
        game_abort("failed to load farR");
    farL = al_load_bitmap("image/player/att_far/farL.png");
    if (!farL)
        game_abort("failed to load farL");
    farU = al_load_bitmap("image/player/att_far/farU.png");
    if (!farU)
        game_abort("failed to load farU");
    for (int i = 0; i < 14; ++i) {
        sprintf(str, "image/player/skill1/invi/invi%d.png", i + 1);
        skill1_invi[i] = al_load_bitmap(str);
        if (!skill1_invi[i])
            game_abort("failed to load invi %d", i);
    }
    for (int i = 0; i < 28; ++i) {
        sprintf(str, "image/player/skill3/skill3_%d.png", i);
        skill3_img[i] = al_load_bitmap(str);
        if (!skill3_img[i])
            game_abort("failed to load skill3 %d", i);
    }
    for (int i = 0; i < 17; ++i) {
        sprintf(str, "image/player/skill2/player_skill2_up_%d.png", i);
        skill2_eff_up[i] = al_load_bitmap(str);
        if (!skill2_eff_up[i])
            game_abort("failed to load skill2 eff up %d", i);
    }
    for (int i = 0; i < 17; ++i) {
        sprintf(str, "image/player/skill2/player_skill2_upR_%d.png", i);
        skill2_eff_upR[i] = al_load_bitmap(str);
        if (!skill2_eff_upR[i])
            game_abort("failed to load skill2 eff upR %d", i);
    }
    for (int i = 0; i < 17; ++i) {
        sprintf(str, "image/player/skill2/player_skill2_down_%d.png", i);
        skill2_eff_down[i] = al_load_bitmap(str);
        if (!skill2_eff_down[i])
            game_abort("failed to load skill2 eff down %d", i);
    }
    for (int i = 0; i < 17; ++i) {
        sprintf(str, "image/player/skill2/player_skill2_downR_%d.png", i);
        skill2_eff_downR[i] = al_load_bitmap(str);
        if (!skill2_eff_downR[i])
            game_abort("failed to load skill2 eff downR %d", i);
    }
    //other
    back = al_load_bitmap("image/back.png");
    if (!back)
        game_abort("failed to load buttom1");
    fader = al_load_bitmap("image/fade.png");
    if (!fader)
        game_abort("failed to load fader");
    skill_fader = al_load_bitmap("image/skill_fader.jpg");
    if (!skill_fader)
        game_abort("failed to load skill fader");
    font = al_load_ttf_font("Caracas Fina.ttf",40,0);
    if (!font)
        game_abort("failed to load font");
    backup = al_load_bitmap("image/stage1_boss/boss1/boss1_skill/die/change.png");
    if (!backup)
        game_abort("failed to load backup");
    stop_img = al_load_bitmap("image/stage1_boss/boss3/boss3_skill/skill5/stop.png");
    if (!stop_img)
        game_abort("failed to load stop");
    setting_img = al_load_bitmap("image/setting/setting.png");
    if (!setting_img)
        game_abort("failed to load setting");
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 2; ++j) {
            sprintf(str, "image/setting/b1-%d-%d.png", i+1,j);
            choose[i][j] = al_load_bitmap(str);
            if (!choose[i][j])
                game_abort("failed to load choose[%d][%d]", i, j);
        }
    }
    for (int i = 0; i < 14; ++i)
    {
        sprintf(str, "image/sum/sum%d.png", i);
        sum[i] = al_load_bitmap(str);
        if (!sum[i])
            game_abort("failed to load sum%d", i);
    }
    for (int i = 0; i < 4; ++i) {
        sprintf(str, "image/hitted/%d.png", i);
        hitted_struck[i] = al_load_bitmap(str);
        if (!hitted_struck[i])
            game_abort("failed to load hitted %d", i);
    }
    //boss
    for (int i = 0; i < 12; ++i)
    {
        sprintf(str, "image/stage1_boss/boss1/boss1_skill/skill1/effect/bo_sk1_eff%d.png", i);
        st1_boss.boss_skill1_eff[i] = al_load_bitmap(str);
        if (!st1_boss.boss_skill1_eff[i])
            game_abort("failed to load skill1 effect %d", i);
    }
    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 18; ++j)
        {
            sprintf(str, "image/stage1_boss/boss1/boss1_skill/skill_pre/CapEffect.areaWarning.%d.png", j);
            lazer_att[i].lazer_pre[j] = al_load_bitmap(str);
            if (!(lazer_att[i].lazer_pre[j]))
                game_abort("failed to load lazer[%d] pre[%d]", i, j);
        }
    }
    for (int i = 0; i < 3; ++i) {
        sprintf(str, "image/stage1_boss/mob_bullet/mob%d.png", i);
        mob_img[i] = al_load_bitmap(str);
        if (!mob_img[i])
            game_abort("failed to load mob %d", i);
        sprintf(str, "image/stage1_boss/mob_bullet/bullet%d.png", i);
        mob_bullet_img[i] = al_load_bitmap(str);
        if (!mob_bullet_img)
            game_abort("failed to load mob bullet img %d", i);
    }
    for (int i = 0; i < 13; ++i) {
        sprintf(str, "image/stage1_boss/skill6_eff/eff%d.png", i);
        skill6_eff[i] = al_load_bitmap(str);
        if (!skill6_eff[i])
            game_abort("failed to load skill6 %d", i);
    }
    st1_boss.boss_w = 222;
    st1_boss.boss_h = 222;
    // sound & bgm
    pick = al_load_sample("sound/pick.mp3");
    if (!pick)
        game_abort("failed to load pick");
    dash_sound = al_load_sample("sound/dash.mp3");
    if(!dash_sound)
        game_abort("failed to load dash sound");
    hitted = al_load_sample("sound/hitted.mp3");
    if (!hitted)
        game_abort("failed to load hitted");
    hit_buttom = al_load_sample("sound/hit_buttom.mp3");
    if (!hit_buttom)
        game_abort("failed to load hit_buttom");
    player_skill1_sound = al_load_sample("sound/player_skill1.mp3");
    if (!player_skill1_sound)
        game_abort("failed to load player_skill1");
    player_skill1_sd_af = al_load_sample("sound/skill1_af.mp3");
    if (!player_skill1_sd_af)
        game_abort("failed to load skill1 af");
    player_skill2_up_sd = al_load_sample("sound/skill2_up.mp3");
    if (!player_skill2_up_sd)
        game_abort("failed to load skill2 up sd");
    player_skill2_down_sd = al_load_sample("sound/skill2_down.mp3");
    if (!player_skill2_down_sd)
        game_abort("failed to load skill2 down sd");
    player_skill3_sound = al_load_sample("sound/player_skill3.mp3");
    if (!player_skill3_sound)
        game_abort("failed to load player_skill3");
    player_skill3_sd_af = al_load_sample("sound/skill3.ogg");
    if (!player_skill3_sd_af)
        game_abort("failed to load skill3 sd");
    st1_boss_skill4_sd = al_load_sample("sound/st1_skill4.mp3");
    if (!st1_boss_skill4_sd)
        game_abort("failed to load st1 skill4 sd");
    st1_boss_skill6_sd = al_load_sample("sound/st1_skill6.mp3");
    if (!st1_boss_skill6_sd)
        game_abort("failed to load st1 skill6 sd");
    flower_sd = al_load_sample("sound/flower.mp3");
    if (!flower_sd)
        game_abort("failed to load flower");
    got_dmg = al_load_sample("sound/got_dmg.mp3");
    if (!got_dmg)
        game_abort("failed to load got dmg");
    //al_play_sample_instance(bgm_instance);
    al_set_sample_instance_playmode(bgm_instance, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(bgm_instance, al_get_default_mixer());
}

void boss1_init() {
    al_unregister_event_source(event_queue, al_get_keyboard_event_source());
    al_unregister_event_source(event_queue, al_get_timer_event_source(timer));
    al_flush_event_queue(event_queue);

    boss_phase = 1;
    st1_boss.radius = 110;
    st1_boss.boss_att = al_load_bitmap("image/stage1_boss/boss1/b1-1_att.png");
    if (!st1_boss.boss_att)
        game_abort("failed to load boss1-att");
    st1_boss.boss_stay = al_load_bitmap("image/stage1_boss/boss1/b1-1_stay.png");
    if (!st1_boss.boss_stay)
        game_abort("failed to load stay");
    st1_boss.boss_skill1 = al_load_bitmap("image/stage1_boss/boss1/boss1_skill/skill1/boss1-blue_skill1.png");
    if (!st1_boss.boss_skill1)
        game_abort("failed to load boss1-blue_skill1.png");
    for (int i = 0; i < 9; ++i)
    {
        lazer_att[i].width = 20;
        for (int j = 0; j < 10; ++j)
        {
            sprintf(str, "image/stage1_boss/boss1/boss1_skill/skill2/bo_sk1_%d.png", j);
            lazer_att[i].lazer_img[j] = al_load_bitmap(str);
            if (!lazer_att[i].lazer_img[j])
                game_abort("failed to load lazer[%d] img[%d]", i, j);
        }
    }
    for (int i = 0; i < 4; ++i)
    {
        sprintf(str, "image/stage1_boss/boss1/boss1_skill/skill4/b1-1_roll%d.png", i);
        st1_boss.boss_skill4[i] = al_load_bitmap(str);
        if (!st1_boss.boss_skill4[i])
            game_abort("failed to load skill4[%d]", i);
    }
    for (int i = 0; i < 6; ++i)
    {
        sprintf(str, "image/stage1_boss/boss1/boss1_skill/die/%d.png", i);
        st1_boss.die[i] = al_load_bitmap(str);
        if (!st1_boss.die[i])
            game_abort("failed to load die[%d]", i);
    }
    st1_boss.boss_skill2 = al_load_bitmap("image/stage1_boss/boss1/b1-1_att.png");
    if (!st1_boss.boss_skill2)
        game_abort("failed to load b1-1_att.png");
    sprintf(str, "image/stage1_boss/boss1/boss1_skill/skill3/bullet.png");
    st1_boss.bullet_img = al_load_bitmap(str);
    if (!st1_boss.bullet_img)
        game_abort("failed to load bullet img");
    flower = al_load_bitmap("image/stage1_boss/boss1/boss1_skill/skill5/skill5.png");
    if (!flower)
        game_abort("failed to load flower");

    st1_boss_skill2_sd = al_load_sample("sound/st1_skill2-1.mp3");
    if (!st1_boss_skill2_sd)
        game_abort("failed to load boss2 skill2 sd");
    BGM = al_load_sample("bgm/Decisive Battle II.flac");
    if (!BGM)
        game_abort("failed to load BGM");
    al_set_sample(bgm_instance,BGM);
    al_draw_bitmap(back, 0, 480, 0);
    al_draw_bitmap(dir ? player[1] : player[0], act_x, act_y, 0);
    al_flip_display();
    restart_init();
    al_rest(1.0);
    game_stage1_sum();
    game_log("stage1 summon success!");
    al_rest(1.5);
    al_play_sample_instance(bgm_instance);
    game_log("Game stage 1 start!");
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    game_stage1();
}

void boss2_init() {
    al_unregister_event_source(event_queue, al_get_keyboard_event_source());
    al_unregister_event_source(event_queue, al_get_timer_event_source(timer));
    al_flush_event_queue(event_queue);
    for (int i = 0; i < 9; i++)
    {
        lazer_att[i].width = 5;
        for (int j = 0; j < 10; ++j) {
            sprintf(str, "image/stage1_boss/boss2/boss2_skill/skill2/lazert%d.png", j);
            lazer_att[i].lazer_img[j] = al_load_bitmap(str);
            if (!lazer_att[i].lazer_img[j])
                game_abort("failed to load lazer %d img%d", i, j);
        }
    }
    st1_boss.boss_att = al_load_bitmap("image/stage1_boss/boss2/b1-2_att.png");
    if (!st1_boss.boss_att)
        game_abort("failed to load boss2 att");
    st1_boss.boss_skill1 = al_load_bitmap("image/stage1_boss/boss2/boss2_skill/skill1/boss1_org_skill1.png");
    if (!st1_boss.boss_skill1)
        game_abort("failed to load boss2 skill1");
    st1_boss.boss_skill2 = al_load_bitmap("image/stage1_boss/boss2/b1-2_att.png");
    if (!st1_boss.boss_skill2)
        game_abort("failed to load boss2 skill2");
    for (int i = 0; i < 4; ++i)
    {
        sprintf(str, "image/stage1_boss/boss2/boss2_skill/skill4/boss1-2_roll%d.png", i);
        st1_boss.boss_skill4[i] = al_load_bitmap(str);
        if (!st1_boss.boss_skill4[i])
            game_abort("failed to load boss2 roll%d", i);
    }
    st1_boss.bullet_img = al_load_bitmap("image/stage1_boss/boss2/boss2_skill/skill3/bullet.png");
    if (!st1_boss.bullet_img)
        game_abort("failed to load bullet image");
    st1_boss.boss_stay = al_load_bitmap("image/stage1_boss/boss2/stay.png");
    if (!st1_boss.boss_stay)
        game_abort("failed to load bullet stay");
    flower = al_load_bitmap("image/stage1_boss/boss2/boss2_skill/skill5/skill5.png");
    if (!flower)
        game_abort("failed to load flower");
    st1_boss_skill2_sd = al_load_sample("sound/st1_skill2-2.mp3");
    for (int i = 0; i < 6; ++i)
    {
        sprintf(str, "image/stage1_boss/boss2/boss2_skill/die/%d.png", i);
        st1_boss.die[i] = al_load_bitmap(str);
        if (!st1_boss.die[i])
            game_abort("failed to load die[%d]", i);
    }
    for (int i = 0; i < 22; ++i) {
        sprintf(str, "image/stage1_boss/boss2/boss2_skill/ult/sun%d.png", i + 1);
        boss2_ult_img[i] = al_load_bitmap(str);
        if (!boss2_ult_img[i])
            game_abort("failed to load sun %d", i);
    }
    for (int i = 0; i < 14; ++i) {
        sprintf(str, "image/stage1_boss/boss2/boss2_skill/ult/anime/sun_anime%d.bmp", i);
        boss2_ult_anime[i] = al_load_bitmap(str);
        if (!boss2_ult_anime[i])
            game_abort("failed to load boss2 ult anime %d", i);
    }
    boss2_ult_back = al_load_bitmap("image/stage1_boss/boss2/boss2_skill/ult/back.png");
    if (!boss2_ult_back)
        game_abort("failed to load ult back");
    st1_boss.boss_y = 920;
    boss_phase = 2;

    boss_ult1_sd =  al_load_sample("sound/ready/boss2_sk2.mp3");
    if (!boss_ult1_sd)
        game_abort("failed to load boss2 ult1");

    BGM2 = al_load_sample("bgm/Bring up Trust.mp3");
    if (!BGM2)
        game_abort("failed to load boss2 bgm");

    al_set_sample(bgm_instance, BGM2);
    al_draw_bitmap(back, 0, 480, 0);
    al_draw_bitmap(dir ? player[1] : player[0], act_x, act_y, 0);
    al_flip_display();
    restart_init();
    game_stage1_sum();
    al_rest(2.0);
    al_play_sample_instance(bgm_instance);
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    game_stage1();
}

void boss3_init() {
    al_unregister_event_source(event_queue, al_get_keyboard_event_source());
    al_unregister_event_source(event_queue, al_get_timer_event_source(timer));
    al_flush_event_queue(event_queue);
    for (int i = 0; i < 9; i++)
    {
        lazer_att[i].width = 10;
        for (int j = 0; j < 10; ++j) {
            sprintf(str, "image/stage1_boss/boss3/boss3_skill/skill2/lazer%d.png", j);
            lazer_att[i].lazer_img[j] = al_load_bitmap(str);
            if (!lazer_att[i].lazer_img[j])
                game_abort("failed to load lazer %d img%d", i, j);
        }
    }
    st1_boss.boss_att = al_load_bitmap("image/stage1_boss/boss3/b1-3_att.png");
    if (!st1_boss.boss_att)
        game_abort("failed to load boss3 att");
    st1_boss.boss_skill1 = al_load_bitmap("image/stage1_boss/boss3/boss3_skill/skill1/skill1.png");
    if (!st1_boss.boss_skill1)
        game_abort("failed to load boss3 skill1");
    st1_boss.boss_skill2 = al_load_bitmap("image/stage1_boss/boss3/b1-3_att.png");
    if (!st1_boss.boss_skill2)
        game_abort("failed to load boss3 skill2");
    for (int i = 0; i < 4; ++i)
    {
        sprintf(str, "image/stage1_boss/boss3/boss3_skill/skill4/skill5_%d.png", i+1);
        st1_boss.boss_skill4[i] = al_load_bitmap(str);
        if (!st1_boss.boss_skill4[i])
            game_abort("failed to load boss3 roll%d", i);
    }
    st1_boss.bullet_img = al_load_bitmap("image/stage1_boss/boss3/boss3_skill/skill3/bullet.png");
    if (!st1_boss.bullet_img)
        game_abort("failed to load bullet image");
    st1_boss.boss_stay = al_load_bitmap("image/stage1_boss/boss3/stay.png");
    if (!st1_boss.boss_stay)
        game_abort("failed to load bullet stay");
    flower = al_load_bitmap("image/stage1_boss/boss3/boss3_skill/skill5/skill5.png");
    if (!flower)
        game_abort("failed to load flower");
    st1_boss_skill2_sd = al_load_sample("sound/st1_skill2-3.mp3");
    for (int i = 0; i < 6; ++i)
    {
        sprintf(str, "image/stage1_boss/boss3/boss3_skill/die/die%d.png", i);
        st1_boss.die[i] = al_load_bitmap(str);
        if (!st1_boss.die[i])
            game_abort("failed to load die[%d]", i);
    }

    st1_boss.boss_y = 920;
    boss_phase = 3;
    
    for (int i = 0; i < 19; ++i) 
    {
        sprintf(str, "image/sum/3sum%d.png", i);
        boss3_sum[i] = al_load_bitmap(str);
        if (!boss3_sum[i])
            game_abort("failed to load sum %d", i);
    }
    boss3_ult1_img = al_load_bitmap("image/stage1_boss/boss3/ult/boss3_ult1.png");
    if (!boss3_ult1_img)
        game_abort("failed to load boss3 ult bullet");
    for (int i = 0; i < 33; ++i) {
        sprintf(str, "image/stage1_boss/boss3/ult/boss3_ult2_t%d.png", i);
        boss3_ult2_img[i] = al_load_bitmap(str);
        if (!boss3_ult2_img[i])
            game_abort("failed to load boss3 ult2 %d", i);
    }
    boss3_ult2_black = al_load_bitmap("image/stage1_boss/boss3/ult/boss3_ultt2.png");
    if (!boss3_ult2_black)
        game_abort("failed to load black");
    stop_sd = al_load_sample("sound/stop.mp3");
    if (!stop_sd)
        game_abort("failed to load stop sd");
    BGM3 = al_load_sample("bgm/Sword of Swords.mp3");
    if (!BGM3)
        game_abort("failed to load boss3 bgm");

    al_set_sample(bgm_instance, BGM3);
    al_draw_bitmap(back, 0, 480, 0);
    al_draw_bitmap(dir ? player[1] : player[0], act_x, act_y, 0);
    al_flip_display();
    al_rest(1.0);
    restart_init();
    for (int i = 0; i < 19; ++i) {
        al_rest(0.12);
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_bitmap(back, 0, 480, 0);
        al_draw_bitmap(player[1], act_x, act_y, 0);
        if (i >= 9) {
            al_draw_bitmap(st1_boss.boss_stay, st1_boss.boss_x, st1_boss.boss_y, 0);
            al_rest(0.05);
        }
        al_draw_bitmap(boss3_sum[i], st1_boss.boss_x-150, st1_boss.boss_y-30, 0);
        al_flip_display();
    }
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_bitmap(back, 0, 480, 0);
    al_draw_bitmap(player[1], act_x, act_y, 0);
    al_draw_bitmap(st1_boss.boss_stay, st1_boss.boss_x, st1_boss.boss_y, 0);
    al_flip_display();
    al_rest(2.0);
    al_play_sample_instance(bgm_instance);
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    game_stage1();
}

void game_stage1_sum() {
    while (sum_ind < 14) {
        al_rest(0.12);
        game_log("timer : %d", sum_ind);
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_bitmap(back, 0, 480, 0);
        al_draw_bitmap(player[1], act_x, act_y, 0);
        if (sum_ind >= 10) {
            al_draw_bitmap(st1_boss.boss_stay, st1_boss.boss_x, st1_boss.boss_y, 0);
            al_rest(0.05);
        }
        al_draw_bitmap(sum[sum_ind % 14], st1_boss.boss_x+50, st1_boss.boss_y+80, 0);
        al_flip_display();
        sum_ind++;
    }
    sum_ind = 0;
}

void game_stage1() {
    while(boss_hp > 0){
        if (boss_phase == 2  && !boss_ult1 && boss_hp <= 100) {
            boss_ult1 = 1;
            boss_att_ind = -5;
            boss2_ult();
        }
        if (boss_phase == 3  && !boss_ult1 && boss_hp <= 200) {
            boss_ult1 = 1;
            boss_att_ind = -5;
            boss3_ult1();
        }
        if (boss_phase == 3 && !boss_ult2 && boss_hp <= 100) {
            boss_ult2 = 1;
            boss_att_ind = -5;
            boss3_ult2();
        }
        boss_att_ind = (rand() % 1000) + 1;
        rand_seed = rand() % 65536;
        game_log("ind %d", boss_att_ind);
        if (now_time - skill6_time >= 1200 + difficulty*600) {
            boss_att_ind = -2;
            st1_boss_skill6();
            skill6_time = now_time;
        }
        if (boss_last_att_ind != 11 && !(boss_att_ind % 11))
        {
            boss_att_ind = -2;
            boss_last_att_ind = 11;
            game_log("boss skill3 used");
            st1_boss_skill3();
        }
        else if (boss_last_att_ind != 9 && !(boss_att_ind % 9))
        {
            boss_att_ind = -2;
            boss_last_att_ind = 9;
            game_log("boss skill5 used");
            st1_boss_skill5();
            st1_boss.boss_y = 920;
        }
        else if (!(boss_att_ind % 5)) {
            for (int i = 0; i < 2; ++i) {
                boss_att_ind = -1;
                boss_last_att_ind = 5;
                game_log("boss skill1 used");
                st1_boss_skill1();
            }
        }
        else if (boss_last_att_ind != 4 && !(boss_att_ind % 4)) {
            boss_att_ind = -2;
            boss_last_att_ind = 4;
            game_log("boss skill2 used");
            st1_boss_skill2();
        }
    }
    if (abs(act_x - st1_boss.boss_x) >= 500) {
        if (act_x > st1_boss.boss_x)
            st1_boss.boss_x = act_x - 500;
        else
            st1_boss.boss_x = act_x + 500;
    }
    score += 800;
    switch (boss_phase) {
    case 1:
        st1_to_2();
        break;
    case 2:
        st2_to_3();
        break;
    case 3:
        end();
    }
}

void st1_boss_skill1() {
    int player_target_x = act_x + 42 - 100;
    int delta_x_displace = player_target_x - st1_boss.boss_x;
    st1_boss.boss_vx = delta_x_displace / 12.;
    st1_boss.boss_vy = 433 / 12.;
    int ct = 0;
    while (ct < 24) {
        if (!freeze && !(ct % 2)) {
            st1_boss.boss_x += st1_boss.boss_vx;
            if (st1_boss.boss_x < 0)
                st1_boss.boss_x = 0;
            else if (st1_boss.boss_x >= 2122)
                st1_boss.boss_x = 2122;
            st1_boss.boss_y -= st1_boss.boss_vy;
        }
        background();
        collision(st1_boss.boss_x, st1_boss.boss_y, st1_boss.radius);
        draw_st1_boss();
        draw_bullet();
        if(freeze)
            al_draw_tinted_bitmap(fader, al_map_rgba(0, 0, 0, 100), 0, 0, NULL);
        drawHP();
        game_loop();
        al_flip_display();
        if(!freeze)
            ct++;
    }
    ct = 0;
    while (ct < 12) {
        if(!freeze)
            st1_boss.boss_y += st1_boss.boss_vy;
        background();
        collision(st1_boss.boss_x, st1_boss.boss_y, st1_boss.radius);
        draw_st1_boss();
        draw_bullet();
        if (freeze)
            al_draw_tinted_bitmap(fader, al_map_rgba(0, 0, 0, 100), 0, 0, NULL);
        drawHP();
        game_loop();
        al_flip_display();
        if(!freeze)
            ct++;
    }
    int skill_x = st1_boss.boss_x;
    int skill_y = st1_boss.boss_y;
    for (int i = 0; i < 12; ) {
        background();
        draw_st1_boss();
        draw_bullet();
        al_draw_bitmap(st1_boss.boss_skill1_eff[i], skill_x + 66, skill_y + 152, 0);
        if (freeze)
            al_draw_tinted_bitmap(fader, al_map_rgba(0, 0, 0, 100), 0, 0, NULL);
        drawHP();
        game_loop();
        al_flip_display();
        if (!freeze)
            ++i;
    }
    al_play_sample(hit_buttom, 5.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
}

void st1_boss_skill2() {
    int lazer_displ = rand() % 100;
    for (int i = 0; i < 9; ++i)
    {
        lazer_att[i].lazer_x = 270 * i + lazer_displ;
    }
   for (int j = 0; j < 17;) {
       if (!freeze && !(now_time % 3)) {
            j++;
        }
       background();
        collision(st1_boss.boss_x , st1_boss.boss_y, st1_boss.radius);
        for(int i=0;i<9;i++)
            al_draw_bitmap(lazer_att[i].lazer_pre[j % 18], lazer_att[i].lazer_x+35, 1050, 0);
        draw_st1_boss();
        draw_bullet();
        if (freeze)
            al_draw_tinted_bitmap(fader, al_map_rgba(0, 0, 0, 100), 0, 0, NULL);
        drawHP();
        game_loop();
        al_flip_display();
    }
   if(boss_phase == 2)
       al_play_sample(st1_boss_skill2_sd, 6.3, 0., 1., ALLEGRO_PLAYMODE_ONCE, 0);
   else
       al_play_sample(st1_boss_skill2_sd, 3.5, 0., 1., ALLEGRO_PLAYMODE_ONCE, 0);
   for (int j = 0; j < 9;) {
       if (!freeze && !(now_time % 8)) {
           j++;
       }
       background();
       collision(st1_boss.boss_x , st1_boss.boss_y, st1_boss.radius);
       for (int i = 0; i < 9; i++) {
            al_draw_bitmap(lazer_att[i].lazer_img[j % 10], lazer_att[i].lazer_x, 450, 0);
            if (!invi && !freeze && now_time - player_got_dmg_time > 100 && (act_x + 50 >= lazer_att[i].lazer_x + lazer_att[i].width
                && act_x + 50 <= lazer_att[i].lazer_x + 170 - lazer_att[i].width)) {
                //((lazer_att[i].lazer_x + lazer_att[i].width > act_x + 50 && lazer_att[i].lazer_x + lazer_att[i].width - (act_x + 50) <= 30) ||
                //   (lazer_att[i].lazer_x + 170 - lazer_att[i].width < act_x + 50 && act_x + 50 - (lazer_att[i].lazer_x + 170 - lazer_att[i].width) <= 27))
                al_play_sample(got_dmg, 770.0, 0., 1., ALLEGRO_PLAYMODE_ONCE, 0);
                score -= 100;
                player_hp--;
                if (player_hp <= 0)
                    again();
                player_got_dmg_time = now_time;
            }
       }
       draw_st1_boss();
       draw_bullet();
       if (freeze)
           al_draw_tinted_bitmap(fader, al_map_rgba(0, 0, 0, 100), 0, 0, NULL);
       drawHP();
       game_loop();
       al_flip_display();
   }
}

void st1_boss_skill3() {
    go_to_mid();
    falling_mush* head = malloc(sizeof(falling_mush));
    head->next_bullet = NULL;
    bullet_max = (rand() % 20) + 40;
    bullet_ct = 0;
    bullet_num = 0;
    do {
        background();
        draw_st1_boss();
        draw_bullet();
        st1_make_bullet(head);
        collision(st1_boss.boss_x , st1_boss.boss_y, st1_boss.radius);
        if (freeze)
            al_draw_tinted_bitmap(fader, al_map_rgba(0, 0, 0, 100), 0, 0, NULL);
        drawHP();
        game_loop();
        al_flip_display();
    } while (bullet_ct > 0);
    free(head);
    go_buttom();
}

/*void st1_boss_skill4() {
    float displace;
    int lim = al_get_timer_count(timer);
    while (al_get_timer_count(timer) - lim < 200) {
        displace = distance(act_x +(float)50,0,st1_boss.boss_x+st1_boss.radius, 0);
        if (displace < 200)
            displace = 200;
        if (act_x > st1_boss.boss_x)
            st1_boss.boss_x += displace/13;
        else
            st1_boss.boss_x -= displace/13;
        if(!(now_time % 10 ))
            al_play_sample(st1_boss_skill4_sd, 5.0, 0., 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
        for (int i = 0; i < 4;)
        {
            background();
            collision(st1_boss.boss_x, st1_boss.boss_y, st1_boss.radius);
            al_draw_bitmap(st1_boss.boss_skill4[i], st1_boss.boss_x, st1_boss.boss_y, 0);
            if (freeze)
                al_draw_tinted_bitmap(fader, al_map_rgba(0, 0, 0, 100), 0, 0, NULL);
            drawHP();
            game_loop();
            al_flip_display();
            if (!freeze)
                ++i;
        }
    }
}*/

void st1_boss_skill5() {
    int player_target_x = act_x + 42 - 100;
    int delta_x_displace = player_target_x - st1_boss.boss_x;
    st1_boss.boss_vx = delta_x_displace / 12.;
    st1_boss.boss_vy = 433 / 12.;
    int ct = 0;
    while (ct < 24) {
        if (!freeze && !(ct % 2)) {
            st1_boss.boss_x += st1_boss.boss_vx;
            if (st1_boss.boss_x < 0)
                st1_boss.boss_x = 0;
            else if (st1_boss.boss_x >= 2122)
                st1_boss.boss_x = 2122;
            st1_boss.boss_y -= st1_boss.boss_vy;
        }
        background();
        collision(st1_boss.boss_x - 135 + st1_boss.radius, st1_boss.boss_y + st1_boss.radius - 150, st1_boss.radius);
        draw_st1_boss();
        draw_bullet();
        if (freeze)
            al_draw_tinted_bitmap(fader, al_map_rgba(0, 0, 0, 100), 0, 0, NULL);
        drawHP();
        game_loop();
        al_flip_display();
        if (!freeze)
            ct++;
    }
    bullet* pre;
    bullet* head = malloc(sizeof(bullet));
    head->next_bullet = NULL;
    switch (boss_phase) {
    case 1:
        bullet_max = (rand() % 20) + 200 - difficulty*80;
        break;
    case 2:
        bullet_max = (rand() % 20) + 50 - difficulty * 20;
        break;
    case 3:
        bullet_max = (rand() % 20) + 55 - difficulty * 30;
        break;
    }
    stop_bullet = now_time;
    bullet_ct = 0;
    bullet_num = 0;
    skill5_lim = al_get_timer_count(timer);
    do{
        for (int i = 0; i < 4;)
        {
            background();
            collision(st1_boss.boss_x - 135 + st1_boss.radius, st1_boss.boss_y + st1_boss.radius - 150, st1_boss.radius);
            al_draw_bitmap(st1_boss.boss_skill4[i], st1_boss.boss_x, st1_boss.boss_y, 0);
            draw_bullet();
            flower_bullet(head);
            if (freeze)
                al_draw_tinted_bitmap(fader, al_map_rgba(0, 0, 0, 100), 0, 0, NULL);
            drawHP();
            game_loop();
            al_flip_display();
            if (!freeze)
                ++i;
        }
    } while (al_get_timer_count(timer) - skill5_lim < 500 && bullet_ct > 0);
    if (bullet_ct)
    {
        while (head) {
            pre = head;
            head = head->next_bullet;
            free(pre);
        }
    }
    ct = 0;
    while (ct < 12) {
        if (!freeze)
            st1_boss.boss_y += st1_boss.boss_vy;
        background();
        collision(st1_boss.boss_x - 135 + st1_boss.radius, st1_boss.boss_y + st1_boss.radius - 150, st1_boss.radius);
        draw_st1_boss();
        draw_bullet();
        if (freeze)
            al_draw_tinted_bitmap(fader, al_map_rgba(0, 0, 0, 100), 0, 0, NULL);
        drawHP();
        game_loop();
        al_flip_display();
        if (!freeze)
            ct++;
    }
    free(head);
}

void st1_boss_skill6() {
    al_play_sample(st1_boss_skill6_sd, 3., 0., 1., ALLEGRO_PLAYMODE_ONCE,0);
    for (int i = 0; i < 13;) {
        background();
        collision(st1_boss.boss_x - 135 + st1_boss.radius, st1_boss.boss_y + st1_boss.radius - 150, st1_boss.radius);
        draw_st1_boss();
        al_draw_bitmap(skill6_eff[i], st1_boss.boss_x - 30, st1_boss.boss_y + 30, 0);
        draw_bullet();
        if (freeze)
            al_draw_tinted_bitmap(fader, al_map_rgba(0, 0, 0, 100), 0, 0, NULL);
        game_loop();
        drawHP();
        al_flip_display();
        if (!freeze && !(now_time%8))
            i++;
    }
    int interval = rand() % 250;
    for (int i = 0; i < 6; ++i) {
        mobs[i].x = 250+interval + 350 * i;
        mobs[i].y = 550;
        mobs[i].hidden = 0;
        mobs[i].hp = 3;
        mobs[i].mod = rand() % boss_phase;
        mobs[i].img = mob_img[mobs[i].mod];
        mobs[i].img_bullet = mob_bullet_img[mobs[i].mod];
        mobs[i].att_cd = now_time + (rand()%50);
    }
}

void boss2_ult() {
    int ct = 0;
    go_to_mid();
    for (int i = 0; i < 14;) {
        background();
        collision(st1_boss.boss_x - 135 + st1_boss.radius, st1_boss.boss_y + st1_boss.radius - 150, st1_boss.radius);
        al_draw_bitmap(boss2_ult_anime[i], camera_x, 487, 0);
        draw_st1_boss();
        draw_bullet();
        if (freeze)
            al_draw_tinted_bitmap(fader, al_map_rgba(0, 0, 0, 100), 0, 0, NULL);
        drawHP();
        game_loop();
        al_flip_display();
        if (!freeze && !(now_time%6))
            i++;
    }
    for (int i = 0; i < 20; ++i) {
        int x = act_x - 250,y = act_y - 300;
        al_play_sample(boss_ult1_sd, 2.0, 0., 1., ALLEGRO_PLAYMODE_ONCE, 0);
        for (int j = 0; j < 22;) {
            background();
            collision(st1_boss.boss_x - 135 + st1_boss.radius, st1_boss.boss_y + st1_boss.radius - 150, st1_boss.radius);
            al_draw_bitmap(boss2_ult_back, camera_x, 487, 0);
            if (j >= 9 && j < 20)
                collision((float)x+50,(float) y + 50,(float) 250);
            draw_st1_boss();
            draw_bullet();
            al_draw_bitmap(boss2_ult_img[j], x, y, 0);
            if (!freeze && !(now_time%3))
                ++j;
            if (freeze)
                al_draw_tinted_bitmap(fader, al_map_rgba(0, 0, 0, 100), 0, 0, NULL);
            drawHP();
            game_loop();
            al_flip_display();
        }
    }
    go_buttom();
}

void boss3_ult1() {
    go_to_mid();
    bullet* head = malloc(sizeof(bullet));
    head->next_bullet = NULL;
    boss3_ult1_time = now_time;
    while (now_time - boss3_ult1_time <= 1200) {
        background();
        collision(st1_boss.boss_x, st1_boss.boss_y, st1_boss.radius);
        draw_st1_boss();
        circle_move(head);
        draw_bullet();
        if (freeze)
            al_draw_tinted_bitmap(fader, al_map_rgba(0, 0, 0, 100), 0, 0, NULL);
        drawHP();
        game_loop();
        al_flip_display();
    }
    go_buttom();
}

void boss3_ult2() {
    go_to_mid();
    for(int i = 0; i < 33;) {
        background();
        collision(st1_boss.boss_x, st1_boss.boss_y, st1_boss.radius);
        draw_st1_boss();
        draw_bullet();
        if (freeze)
            al_draw_tinted_bitmap(fader, al_map_rgba(0, 0, 0, 100), 0, 0, NULL);
        drawHP();
        game_loop();
        al_draw_bitmap(boss3_ult2_img[i], camera_x, 450, 0);
        if (!(now_time % 7))
            i++;
        al_flip_display();
    }
    get_small = 1;
    go_buttom();
}

void st1_to_2() {
    al_stop_sample_instance(bgm_instance);

    for (int i = 0; i < ALLEGRO_KEY_MAX; ++i)
        key_state[i] = 0;

    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_bitmap(back, 0, 480, 0);
    al_draw_bitmap(dir ? player[1] : player[0], act_x, act_y, 0);
    al_flip_display();

    while (sum_ind < 6) {
        al_rest(0.12);
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_bitmap(back, 0, 480, 0);
        al_draw_bitmap(dir ? player[1] : player[0], act_x, act_y, 0);
        al_draw_bitmap(st1_boss.die[sum_ind], st1_boss.boss_x+30, st1_boss.boss_y+70, 0);
        al_flip_display();
        sum_ind++;
    }
    sum_ind = 0;

    al_draw_bitmap(back, 0, 480, 0);
    al_draw_bitmap(dir ? player[1] : player[0], act_x, act_y, 0);
    al_draw_bitmap(st1_boss.die[5],st1_boss.boss_x + 30, st1_boss.boss_y+70, 0);
    al_flip_display();
    al_rest(1.0);
    // change
    for (float i = 0; i < 15; ++i)
    {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_bitmap(back, 0, 480, 0);
        al_draw_bitmap(dir ? player[1] : player[0], act_x, act_y, 0);
        al_draw_bitmap(st1_boss.die[5], st1_boss.boss_x + 30, st1_boss.boss_y+70, 0);
        al_flip_display();
        al_rest(0.3 - i *2/100);
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_bitmap(back, 0, 480, 0);
        al_draw_bitmap(dir ? player[1] : player[0], act_x, act_y, 0);
        al_draw_bitmap(backup, st1_boss.boss_x, st1_boss.boss_y+145, 0);
        al_flip_display();
        al_rest(0.3 - i *2/ 100);
    }
    al_rest(1.0);
    game_log("boss change!!");
    destroy1();
    boss2_init();
}

void st2_to_3() {
    al_stop_sample_instance(bgm_instance);

    for (int i = 0; i < ALLEGRO_KEY_MAX; ++i)
        key_state[i] = 0;

    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_bitmap(back, 0, 480, 0);
    al_draw_bitmap(dir ? player[1] : player[0], act_x, act_y, 0);
    al_flip_display();

    while (sum_ind < 6) {
        al_rest(0.12);
        game_log("timer : %d", sum_ind);
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_bitmap(back, 0, 480, 0);
        al_draw_bitmap(dir ? player[1] : player[0], act_x, act_y, 0);
        al_draw_bitmap(st1_boss.die[sum_ind], st1_boss.boss_x + 30, st1_boss.boss_y + 70, 0);
        al_flip_display();
        sum_ind++;
    }
    st1_boss.boss_y = 920;
    boss_hp = 200;
    sum_ind = 0;
    boss_phase = 1;

    al_draw_bitmap(back, 0, 480, 0);
    al_draw_bitmap(dir ? player[1] : player[0], act_x, act_y, 0);
    al_draw_bitmap(st1_boss.die[5], st1_boss.boss_x + 30, st1_boss.boss_y + 70, 0);
    al_flip_display();  
    destroy2();
    boss3_init();
}

void draw_st1_boss() {
    if (boss_att_ind == -1)
        al_draw_bitmap(st1_boss.boss_skill1, st1_boss.boss_x, st1_boss.boss_y, 0);
    else
        al_draw_bitmap(st1_boss.boss_skill2, st1_boss.boss_x, st1_boss.boss_y, 0);
}

void game_loop() {
            player_move();
            camera_update();
            player_draw();
}

float jump_up() {
    jump_v = jump_v * 4.95 / 5.2;
    return jump_v;
}

float jump_down() {
    jump_v = jump_v * 5.3 / 4.95;
    return jump_v;
}

void player_move() {
    now_time = al_get_timer_count(timer);
    float vx = 0;
    float vy = 0.1;
    if (now_time - invi > 350)
        invi = 0;
    if (!skill1 && now_time - skill1_cd > 1430)
        skill1 = 1;
    if (key_state[ALLEGRO_KEY_A] && skill1) {
        al_play_sample(player_skill1_sound,5.0,0.0,0.9,ALLEGRO_PLAYMODE_ONCE,0);
        al_unregister_event_source(event_queue, al_get_keyboard_event_source());
        key_state[ALLEGRO_KEY_A] = 0;
        key_state[ALLEGRO_KEY_RIGHT] = 0;
        key_state[ALLEGRO_KEY_LEFT] = 0;
        game_log("skill1! used %d",al_get_timer_count(timer));
        freeze = 1;
        skill1 = 0;
        invi = now_time;
        skill1_cd = now_time;
        skill_ind = 1;
        skill5_lim += 230;
        stop_bullet += 230;
        boss3_ult1_time += 230;
    }
    if (player_mp == 50 && key_state[ALLEGRO_KEY_D]) {
        al_play_sample(player_skill3_sound, 4.0, 0.0, 0.9, ALLEGRO_PLAYMODE_ONCE, 0);
        al_unregister_event_source(event_queue, al_get_keyboard_event_source());
        key_state[ALLEGRO_KEY_D] = 0;
        key_state[ALLEGRO_KEY_RIGHT] = 0;
        key_state[ALLEGRO_KEY_LEFT] = 0;
        game_log("skill3! used %d", al_get_timer_count(timer));
        player_mp = 0;
        freeze = 1;
        skill_ind = 3;
        skill5_lim += 230;
        stop_bullet += 230;
        boss3_ult1_time += 230;
    }
    if (player_mp >= 25 && key_state[ALLEGRO_KEY_S]) {
        al_play_sample(player_skill3_sound, 4.0, 0.0, 0.9, ALLEGRO_PLAYMODE_ONCE, 0);
        al_unregister_event_source(event_queue, al_get_keyboard_event_source());
        key_state[ALLEGRO_KEY_S] = 0;
        key_state[ALLEGRO_KEY_RIGHT] = 0;
        key_state[ALLEGRO_KEY_LEFT] = 0;
        game_log("skill2! used %d", al_get_timer_count(timer));
        player_mp -= 25;
        freeze = 1;
        skill_ind = 2;
        skill5_lim += 230;
        stop_bullet += 230;
        boss3_ult1_time += 230;
    }
    if (freeze) return;
    if (can_move) {
        if (can_up && key_state[ALLEGRO_KEY_Z])
            vy = -jump_up();
        else if (act_y <= bottom)
            vy = jump_down();
        if (key_state[ALLEGRO_KEY_LEFT]) {
            vx = -1;
            dir = 0;
        }
        if (key_state[ALLEGRO_KEY_RIGHT]) {
            vx = 1;
            dir = 1;
        }
    }
    if (key_state[ALLEGRO_KEY_C]&& now_time - last_dash > 50 && can_dash  ) {
        game_log("Dash!!");
        al_play_sample(dash_sound, 150.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE,0);
        last_dash = now_time;
        can_move = 0;
        can_dash = 0;
        can_up = 0;
    }
    if (!can_dash && dash_v > 1.2) {
        vx = dash();
    }
    else {
        can_move = 1;
    }
    if (act_x + 93. + vx * 8 <= screen_W + R_bound && act_x + vx*8 >= 0)
        act_x += vx * 8;
    if (act_y + 46. + vy * 8 * 2 <= screen_H + 720. && act_y + vy*8> 0)
        act_y += vy * 8;
    if (act_y <= 820 || !key_state[ALLEGRO_KEY_Z]) {
        if (can_up)
            jump_v = 0.65;
        can_up = 0;
    }
    if (act_y >= 1030 && !key_state[ALLEGRO_KEY_Z]) {
        can_up = 1;
        jump_v = 1.7;
        if (!can_dash && dash_v < 1.2) {
            can_dash = 1;
            dash_v = 3.0;
        }
    }

    if (can_move) {
        if (key_state[ALLEGRO_KEY_X] && now_time - last_att > 17) {
            for (int i = 0; i < 4; ++i) {
                if (sword_wave[i].hidden) {
                    game_log("att!");
                    sword_wave[i].start = act_x;
                    last_att = now_time;
                    sword_wave[i].hidden = 0;
                    if (key_state[ALLEGRO_KEY_UP]) {
                        sword_wave[i].x = act_x;
                        sword_wave[i].y = act_y + sword_wave[i].radius * 2;
                        sword_wave[i].vx = 0;
                        sword_wave[i].vy = -13;
                        sword_wave[i].img = farU;
                    }
                    else if (dir) {
                        sword_wave[i].x = act_x + sword_wave[i].radius * 2;
                        sword_wave[i].y = act_y;
                        sword_wave[i].vx = 13;
                        sword_wave[i].vy = 0;
                        sword_wave[i].img = farR;
                    }
                    else {
                        sword_wave[i].x = act_x - sword_wave[i].radius * 2;
                        sword_wave[i].y = act_y;
                        sword_wave[i].vx = -13;
                        sword_wave[i].vy = 0;
                        sword_wave[i].img = farL;
                    }
                    break;
                }
            }
        }
    }
}

void camera_update() {
    camera_x = -(screen_W / 2) + act_x + 12 / 2;
    //camera_y = -(8*screen_H / 10) + 1050 + 26 / 2
    if (camera_x < 0)
        camera_x = 0;
    if (camera_x > 1080)
        camera_x = 1080;
    al_identity_transform(&camera);
    al_translate_transform(&camera, -camera_x, -487);
    al_use_transform(&camera);
}

void player_draw() {
    //skill anime
    if (freeze) {
        if (skill_ind != 2 || skill_pre_ind < 37) {
            if (!(now_time % 3))
                player_skill_ind++;
            al_draw_bitmap(player_skill1_img[player_skill_ind % 6], act_x + 25, act_y - 20, 0);
        }
        if (skill_pre_ind < 37) {
            if (skill_pre_ind < 26) {
                if (!(now_time % 3))
                    skill_pre_ind++;
                al_draw_bitmap(skill1_pre[skill_pre_ind], act_x + 5, act_y - 20, 0);
            }
            else {
                if (!(now_time % 3))
                    skill_pre_ind++;
                al_draw_bitmap(skill1_pre[skill_pre_ind], act_x - 160, act_y - 125, 0);
            }
        }
        else if (skill_ind == 1) {
            if (skill_pre_ind >= 37 && skill_eff_ind < 28) {
                if (!(skill1_af_ind % 2)) {
                    skill1_af_ind++;
                    al_play_sample(player_skill1_sd_af, 4., 0., 1., ALLEGRO_PLAYMODE_ONCE, 0);
                }
                if (!(now_time % 4))
                    skill_eff_ind++;
                al_draw_bitmap(skill1_eff[skill_eff_ind], camera_x, 400, 0);
            }
            else if (skill_eff_ind == 28) {
                al_register_event_source(event_queue, al_get_keyboard_event_source());
                freeze = 0;
                skill_pre_ind = 0;
                skill_eff_ind = 0;
                skill1_af_ind++;
            }
        }
        else if (skill_ind == 3) {
            if (skill_pre_ind >= 37 && skill_eff_ind < 27) {
                if (!(skill1_af_ind % 2)) {
                    skill1_af_ind++;
                    al_play_sample(player_skill3_sd_af, 4., 0., 0.8, ALLEGRO_PLAYMODE_ONCE, 0);
                }
                al_draw_bitmap(skill3_img[skill_eff_ind], camera_x+200, 400, 0);
                if (!(now_time % 4))
                    skill_eff_ind++;
            }
            else if (skill_eff_ind == 27) {
                al_register_event_source(event_queue, al_get_keyboard_event_source());
                player_hp += 2;
                if (difficulty) {
                    if (player_hp >= 10)
                        player_hp = 10;
                }
                else {
                    if (player_hp >= 6)
                        player_hp = 6;
                }
                freeze = 0;
                skill_pre_ind = 0;
                skill_eff_ind = 0;
                skill1_af_ind++;
            }
        }
        else if (skill_ind == 2) {
            if (skill_pre_ind >= 37 && skill_eff_ind < 32) {
                if (!dir) {
                    if (skill_eff_ind < 16) {
                        if (!(skill1_af_ind % 2)) {
                            al_play_sample(player_skill2_up_sd, 3., 0., 1., ALLEGRO_PLAYMODE_ONCE, 0);
                            skill1_af_ind++;
                        }
                        al_draw_bitmap(player_attL[player_attL_ind % 6], act_x, act_y, 0);
                        if (!(now_time % 8))
                            player_attL_ind++;
                        al_draw_bitmap(skill2_eff_up[skill_eff_ind], act_x - 450, act_y + 90 - 480, 0);
                        if (!(now_time % 3))
                            skill_eff_ind++;
                    }
                    else {
                        if (skill1_af_ind%2) {
                            al_play_sample(player_skill2_down_sd, 3., 0., 1., ALLEGRO_PLAYMODE_ONCE, 0);
                            skill1_af_ind++;
                        }
                        al_draw_bitmap(skill2_img_movL[player_skill2_ind % 6], act_x + 25, act_y - 5, 0);
                        if (!(now_time % 8))
                            player_skill2_ind++;
                        al_draw_bitmap(skill2_eff_down[skill_eff_ind % 16], act_x - 450, act_y + 90 - 370, 0);
                        if (!(now_time % 3))
                            skill_eff_ind++;
                    }
                }
                else {
                    if (skill_eff_ind < 16) {
                        if (!(skill1_af_ind % 2)) {
                            al_play_sample(player_skill2_up_sd, 3., 0., 1., ALLEGRO_PLAYMODE_ONCE, 0);
                            skill1_af_ind++;
                        }
                        al_draw_bitmap(player_attR[player_attR_ind % 6], act_x, act_y, 0);
                        if (!(now_time % 8))
                            player_attR_ind++;
                        al_draw_bitmap(skill2_eff_upR[skill_eff_ind], act_x-20, act_y + 90 - 480, 0);
                        if (!(now_time % 3))
                            skill_eff_ind++;
                    }
                    else {
                        if (skill1_af_ind % 2) {
                            al_play_sample(player_skill2_down_sd, 3., 0., 1., ALLEGRO_PLAYMODE_ONCE, 0);
                            skill1_af_ind++;
                        }
                        al_draw_bitmap(skill2_img_mov[player_skill2_ind % 6], act_x + 25, act_y - 5, 0);
                        if (!(now_time % 8))
                            player_skill2_ind++;
                        al_draw_bitmap(skill2_eff_downR[skill_eff_ind%16], act_x-20, act_y + 90 - 370, 0);
                        if (!(now_time % 3))
                            skill_eff_ind++;
                    }
                }
            }
            else if (skill_eff_ind == 32) {
                al_register_event_source(event_queue, al_get_keyboard_event_source());
                if (dir && distance(act_x - 20 + 190+350, act_y + 90 - 370+350, st1_boss.boss_x+st1_boss.radius,st1_boss.boss_y+st1_boss.radius)
                    <= 350 + st1_boss.radius+50) {
                    boss_hp -= 20;
                    if (boss_hp < 0)
                        boss_hp = 0;
                    al_play_sample(hitted, 2.3, 0., 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                }
                else if (!dir && distance(act_x - 450 + 350, act_y + 90 - 370 + 350, st1_boss.boss_x + st1_boss.radius, st1_boss.boss_y + st1_boss.radius)
                    < +350 + st1_boss.radius + 50) {
                    boss_hp -= 20+difficulty*10;
                    if (boss_hp < 0)
                        boss_hp = 0;
                    al_play_sample(hitted, 2.3, 0., 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                }
                freeze = 0;
                player_attR_ind = 0;
                skill_pre_ind = 0;
                skill_eff_ind = 0;
                skill1_af_ind = 0;
                player_skill2_ind = 0;
            }
        }
        return;
    }
    if (invi)
        al_draw_bitmap(skill1_invi[invi_ind++%14], act_x, act_y + 30, 0);
    now_time = al_get_timer_count(timer);
    if (!(now_time - player_got_dmg_time < 100 && !(now_time % 3))) {
        if (can_move)
        {
            if (now_time - last_att < 31)
            {
                if (!(now_time % 5))
                    if (dir)
                        player_attR_ind++;
                    else
                        player_attL_ind++;
                al_draw_bitmap(dir ? player_attR[player_attR_ind % 6] : player_attL[player_attL_ind % 6], act_x, act_y, 0);
            }
            else if (act_y <= bottom) {
                al_draw_bitmap(dir ? player_jump[1] : player_jump[0], act_x, act_y, 0);
            }
            else if (key_state[ALLEGRO_KEY_RIGHT]) {
                if (!(now_time % 7))
                    playerR_ind++;
                al_draw_bitmap(playerR[playerR_ind % 5], act_x, act_y, 0);
            }
            else if (key_state[ALLEGRO_KEY_LEFT]) {
                if (!(now_time % 7))
                    playerL_ind++;
                al_draw_bitmap(playerL[playerL_ind % 5], act_x, act_y, 0);
            }
            else if (dir)
                al_draw_bitmap(player[1], act_x, act_y, 0);
            else
                al_draw_bitmap(player[0], act_x, act_y, 0);
        }
        else {
            al_draw_bitmap(dir ? player_dash[1] : player_dash[0], act_x, act_y, 0);
        }
        if (!can_dash && dash_v >= 1.2) {
            if (dir) {
                if (!(now_time % 4))
                    player_dash_ind++;
                al_draw_bitmap(player_dash_img[1][player_dash_ind % 6], act_x - 90, act_y + 25, 0);
            }
            else {
                if (!(now_time % 4))
                    player_dash_ind++;
                al_draw_bitmap(player_dash_img[0][player_dash_ind % 6], act_x + 90, act_y + 25, 0);
            }
        }
    }
    if (!freeze && get_small && boss3_ult2_ind<1050 - difficulty*150)
        boss3_ult2_ind++;
    
}

void restart_init() {
    now_time = al_get_timer_count(timer);
    if (st1_boss.boss_x == -1000)
    {
        st1_boss.boss_x = 800;
        st1_boss.boss_y = 920;
        st1_boss.boss_vx = 0;
        st1_boss.boss_vy = 0;
        st1_boss.radius = 110;
        act_x = 400;
        act_y = 1050;
    }
    for (int i = 0; i < 6; ++i) {
        mobs[i].hidden = 1;
    }
    for (int i = 0; i < 50; ++i)
        mobs_bullet[i].hidden = 1;
    for (int i = 0; i < 4; ++i)
        sword_wave[i].hidden = 1;
    player_hp = difficulty ? 10 : 6;
    player_mp = 0;
    player_got_dmg_time = -50;
    player_skill_ind = 0;
    invi = 0;
    skill1_af_ind = 0;
    R_bound = 1080;
    boss_att_time = -100;
    boss_att_ind = 0;
    skill6_time = now_time;
    boss_hp = boss_phase == 3 ? 300 : 200;
    boss_last_att_ind = -1;
    bullet_num = 0;
    bullet_ct = 0;
    freeze = 0;
    boss_get_dmg = 1;
    last_boss_dmg = -10;
    boss_ult1 = 0;
    boss_ult2 = 0;
    get_small = 0;
    boss3_ult2_ind = 0;
    flower_stop = 0;
    can_dash = 1;
    last_dash = 0;
    dir = 1;
    can_move = 1;
    show_norm_att = 0;
    can_up = 1;
    sum_ind = 0;
    skill1_cd = now_time;
    skill1 = 0;
    hitted_ind = 4;
}

void setting() {
    while (1) {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_text(font,al_map_rgb(255,255,255),10, 500, 0,"Back");
        al_draw_text(font, al_map_rgb(255, 255, 255), 150, 600, 0, "Boss1               Boss2              Boss3");
        if (difficulty)
            al_draw_text(font, al_map_rgb(0, 255, 0), 600, 500, ALLEGRO_ALIGN_CENTRE, "Easy");
        else
            al_draw_text(font, al_map_rgb(255, 0, 0), 600, 500, ALLEGRO_ALIGN_CENTRE, "Hard");
        al_draw_text(font, al_map_rgb(0, 255, 0), 400, 1000, ALLEGRO_ALIGN_CENTRE, "Easy");
        al_draw_text(font, al_map_rgb(255, 0, 0), 850, 1000, ALLEGRO_ALIGN_CENTRE, "Hard");
        if(mouse_x >= 110 && mouse_x <=338 && mouse_y >=180 && mouse_y <=400)
            al_draw_bitmap(choose[0][0], 110, 650, 0);
        else
            al_draw_bitmap(choose[0][1], 110, 650, 0);
        if (mouse_x >= 540 && mouse_x <= 778 && mouse_y >= 180 && mouse_y <= 400)
            al_draw_bitmap(choose[1][0], 540, 650, 0);
        else
            al_draw_bitmap(choose[1][1], 540, 650, 0);
        if (mouse_x >= 960 && mouse_x <= 1198 && mouse_y >= 180 && mouse_y <= 400)
            al_draw_bitmap(choose[2][0], 960, 650, 0);
        else
            al_draw_bitmap(choose[2][1], 960, 650, 0);
        al_wait_for_event(event_queue, &event);
        if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouse_x = event.mouse.x;
            mouse_y = event.mouse.y;
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            game_log("%d %d", mouse_x, mouse_y);
            if (mouse_x >= 10 && mouse_x <= 140 && mouse_y <= 50 && mouse_y >= 25) {
                al_play_sample(pick, 2.5, 0., 1., ALLEGRO_PLAYMODE_ONCE, 0);
                return;
            }
            else if (mouse_x >= 110 && mouse_x <= 338 && mouse_y >= 180 && mouse_y <= 400)
                boss1_init();
            else if (mouse_x >= 540 && mouse_x <= 778 && mouse_y >= 180 && mouse_y <= 400)
                boss2_init();
            else if (mouse_x >= 960 && mouse_x <= 1198 && mouse_y >= 180 && mouse_y <= 400)
                boss3_init();
            else if (mouse_x >= 350 && mouse_x <= 450 && mouse_y >= 520 && mouse_y <= 550)
                difficulty = 1;
            else if(mouse_x >= 800 && mouse_x <= 900 && mouse_y >= 520 && mouse_y <= 550)
                difficulty = 0;
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            game_log("game end");
            exit(1);
        }
        al_flip_display();
    }
}

void beginning() {
    sprintf(str, "Death : %d", die);
    st1_boss.boss_x = -1000;
    score = 0;
    act_x = 400;
    camera_update();
    while (1) {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_text(font, al_map_rgb(255, 255, 255), 640, 600, ALLEGRO_ALIGN_CENTRE, str);
        //al_draw_text(font, al_map_rgb(255, 255, 255), 640, 550, ALLEGRO_ALIGN_CENTRE, "if you can't move, change the input methon");
        al_draw_text(font, al_map_rgb(255, 255, 255), 630, 700, 0, "(cost 50% mp) (cost 100% mp)");
        al_draw_text(font, al_map_rgb(255, 255, 255), 640, 750, ALLEGRO_ALIGN_CENTRE, "A : invincible for 2s(cd 20s) S : attack skill  D : heal 2 hp");
        al_draw_text(font, al_map_rgb(255, 255, 255), 640, 850, ALLEGRO_ALIGN_CENTRE, "Z : jump   X : attack   C : dash");
        al_draw_text(font, al_map_rgb(255, 255, 255), 640, 950, ALLEGRO_ALIGN_CENTRE, "Press space to start the game");
        //al_draw_text(font, al_map_rgb(255, 0, 0), 640, 1050, ALLEGRO_ALIGN_CENTRE, "Turn down the volume first!!!!!!!");
        al_draw_bitmap(setting_img, 10, 500, 0);
        al_flip_display();
        al_wait_for_event(event_queue, &event);
        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                game_log("begin continue");
                al_clear_to_color(al_map_rgb(0, 0, 0));
                al_flip_display();
                al_rest(1.0);
                al_unregister_event_source(event_queue, al_get_keyboard_event_source());
                al_unregister_event_source(event_queue, al_get_timer_event_source(timer));
                al_flush_event_queue(event_queue);
                boss1_init();
            }
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouse_x = event.mouse.x;
            mouse_y = event.mouse.y;
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            game_log("%d %d", mouse_x, mouse_y);
            if (mouse_x >= 10 && mouse_x <= 110 && mouse_y <= 110 && mouse_y >= 10) {
                al_play_sample(pick, 2.5, 0., 1., ALLEGRO_PLAYMODE_ONCE, 0);
                setting();
            }
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            game_log("game end");
            exit(1);
        }
    }
}

void again() {
    al_stop_sample_instance(bgm_instance);
    score = 0;
    die++;
    sprintf(str, "Die : %d", die);
    for (int i = 0; i < ALLEGRO_KEY_MAX; ++i)
        key_state[i] = 0;
    while (1) {
        al_wait_for_event(event_queue, &event);
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_bitmap(back, 0, 480, 0);
        al_draw_bitmap(player[1], act_x, act_y, 0);
        al_draw_bitmap(st1_boss.boss_stay, st1_boss.boss_x, st1_boss.boss_y, 0);
        al_draw_tinted_bitmap(fader, al_map_rgba(0, 0, 0, 100), 0, 0, NULL);
        al_draw_text(font, al_map_rgb(255, 255, 255), camera_x + 640, 700, ALLEGRO_ALIGN_CENTRE, str);
        al_draw_text(font, al_map_rgb(255, 255, 255), camera_x + 640, 800, ALLEGRO_ALIGN_CENTRE, "Again?");
        al_draw_text(font, al_map_rgb(255, 255, 255), camera_x + 640, 900, ALLEGRO_ALIGN_CENTRE, "Back to menu");
        al_flip_display();
        if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouse_x = event.mouse.x;
            mouse_y = event.mouse.y;
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            game_log("%d %d", mouse_x, mouse_y);
            if (mouse_x >= 500 && mouse_x <= 680 && mouse_y >= 315 && mouse_y <= 350) {
                st1_boss.boss_x = -1000;
                restart_init();
                camera_update();
                al_flip_display();
                switch (boss_phase) {
                case 1:
                    destroy1();
                    boss1_init();
                    break;
                case 2:
                    destroy2();
                    boss2_init();
                    break;
                case 3:
                    destroy3();
                    boss3_init();
                    break;
                }
            }
            if (mouse_x >= 400 && mouse_x <= 780 && mouse_y >= 420 && mouse_y <= 450)
                beginning();
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            game_log("game end");
            exit(1);
        }
    }
}

void end() {
    st1_boss.boss_x = -1000;
    act_x = 400;
    camera_update();
    char dies[10];
    sprintf(dies, "Die : %d", die);
    sprintf(str, "Score : %d", score);
    for (int i = 0; i < ALLEGRO_KEY_MAX; ++i)
        key_state[i] = 0;
    while (1) {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        if (difficulty)
            al_draw_text(font, al_map_rgb(0, 255, 0), 650, 700, ALLEGRO_ALIGN_CENTRE, "Easy");
        else
            al_draw_text(font, al_map_rgb(255, 0, 0), 650, 700, ALLEGRO_ALIGN_CENTRE, "Hard");
        al_draw_text(font, al_map_rgb(255, 255, 255), 640, 550, ALLEGRO_ALIGN_CENTRE, "You Win!!");
        al_draw_text(font, al_map_rgb(255, 255, 255), 640, 750, ALLEGRO_ALIGN_CENTRE,str);
        al_draw_text(font, al_map_rgb(255, 255, 255), 640, 850, ALLEGRO_ALIGN_CENTRE, dies);
        al_draw_text(font, al_map_rgb(255, 255, 255), 640, 950, ALLEGRO_ALIGN_CENTRE, "Back to manu");
        al_flip_display();
        al_wait_for_event(event_queue, &event);
        if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouse_x = event.mouse.x;
            mouse_y = event.mouse.y;
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            game_log("%d %d", mouse_x, mouse_y);
            if (mouse_x >= 400 && mouse_x <= 790 && mouse_y >= 470 && mouse_y <= 500) {
                al_stop_sample_instance(bgm_instance);
                destroy3();
                beginning();
            }
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            game_log("game end");
            exit(1);
        }
    }
}

void go_to_mid() {
    int tar_x = 2360 / 2, tar_y = (1040 + 487) / 2;
    st1_boss.boss_vx = (tar_x - st1_boss.boss_x) / 10 / 2;
    st1_boss.boss_vy = (tar_y - 50 - st1_boss.boss_y) / 10 / 2;
    int ct = 0;
    while (ct < 20) {
        if (!freeze && !(now_time % 2)) {
            st1_boss.boss_x += st1_boss.boss_vx;
            st1_boss.boss_y += st1_boss.boss_vy;
            ct++;
        }
        background();
        collision(st1_boss.boss_x - 135 + st1_boss.radius, st1_boss.boss_y + st1_boss.radius - 150, st1_boss.radius);
        draw_st1_boss();
        draw_bullet();
        if (freeze)
            al_draw_tinted_bitmap(fader, al_map_rgba(0, 0, 0, 100), 0, 0, NULL);
        drawHP();
        game_loop();
        al_flip_display();
    }
}

void go_buttom() {
    int ct = 0;
    while (ct < 20) {
        background();
        collision(st1_boss.boss_x - 135 + st1_boss.radius, st1_boss.boss_y + st1_boss.radius - 150, st1_boss.radius);
        draw_st1_boss();
        draw_bullet();
        if (freeze)
            al_draw_tinted_bitmap(fader, al_map_rgba(0, 0, 0, 100), 0, 0, NULL);
        drawHP();
        game_loop();
        al_flip_display();
        if (!freeze) {
            ct++;
            st1_boss.boss_y -= st1_boss.boss_vy;
        }
    }
}

void background() {
    st:
    al_wait_for_event(event_queue, &event);
    if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        game_log("game end");
        exit(1);
    }
    else if (event.type == ALLEGRO_EVENT_KEY_DOWN && !key_state[event.keyboard.keycode]) {
        game_log("key %d down", event.keyboard.keycode);
        key_state[event.keyboard.keycode] = true;
    }
    else if (event.type == ALLEGRO_EVENT_KEY_UP) {
        game_log("key %d up", event.keyboard.keycode);
        key_state[event.keyboard.keycode] = false;
    }
    else {
            if (!(event.type == ALLEGRO_EVENT_TIMER)){
                goto st;
        }
    }
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_bitmap(back, 0, 480, 0);
}

void drawHP() {
    if(boss_phase == 3)
        al_draw_scaled_bitmap(boss3_ult2_black, 0, 0, 2000, 1327, act_x + 50 - 2000. * 14. / 2 + boss3_ult2_ind * 2000. / 90. / 2, act_y + 35 - 1327 * 14. / 2 + boss3_ult2_ind * 1327. / 90 / 2, 2000 * 14 - boss3_ult2_ind * 2000. / 90,1327 * 14 - boss3_ult2_ind * 1327. / 90, 0);
    for (int i = 0; i < player_hp; ++i)
    {
        al_draw_filled_rectangle(camera_x + i * 80 + 50, 500, camera_x + i * 80 + 50 + 50, 550, al_map_rgb(227, 23, 13));
    }
    if (player_mp > 50)
        player_mp = 50;
    al_draw_rectangle(camera_x + 50, 550., camera_x +50+ 200., 565., al_map_rgb(0, 255, 255), 1);
    al_draw_filled_rectangle(camera_x + 50.,550, camera_x +50 + (player_mp / 50.) * (200.), 565., al_map_rgb(0, 255, 255));
    sprintf(str, "Score : %d", score);
    al_draw_text(font, al_map_rgb(255, 255, 255), camera_x+900, 500,0, str);
    for (int i = 0; i < 3; ++i) {
        al_draw_bitmap(skill_img[i], camera_x + 50 + i*70, 575, 0);
    }
    if(player_mp < 25)
        al_draw_tinted_bitmap(skill_fader, al_map_rgba(0, 0, 0, 100), camera_x + 120, 575, NULL);
    if (player_mp < 50)
        al_draw_tinted_bitmap(skill_fader, al_map_rgba(0, 0, 0, 100), camera_x + 190, 575, NULL);
    if(1430. - (now_time - skill1_cd) > 0)
        al_draw_filled_rectangle(camera_x + 50, 575, camera_x + 50 +(1430.-(now_time- skill1_cd))/1430.*60., 575+60, al_map_rgb(0, 0, 0));
    al_draw_rectangle(camera_x + 300, 1170., camera_x + 900., 1190., al_map_rgb(227, 23, 13), 1);
    if (boss_hp < 200)
        al_draw_filled_rectangle(camera_x + 300, 1170., camera_x + 300 + (boss_hp / 200.) * (600.), 1190., al_map_rgb(227, 23, 13));
    else
        al_draw_filled_rectangle(camera_x + 300, 1170., camera_x + 300 + (600.), 1190., al_map_rgb(227, 23, 13));
    if (boss_hp > 200)
        al_draw_filled_rectangle(camera_x + 300, 1170., camera_x + 300 + ((boss_hp - 200) / 200.) * (600.), 1190., al_map_rgb(0, 255, 0));
}

void draw_bullet() {
    // player 判定
    now_time = al_get_timer_count(timer);
    for (int i = 0; i < 4; ++i){
        if (freeze || sword_wave[i].hidden) continue;
        sword_wave[i].x += sword_wave[i].vx;
        sword_wave[i].y += sword_wave[i].vy;
        if (sword_wave[i].x < -5 || sword_wave[i].x >= 2300 || sword_wave[i].y <= 465 || abs(sword_wave[i].start - sword_wave[i].x) > 550) {
            sword_wave[i].hidden = 1;
            continue;
        }
        for (int j = 0; j < 6; ++j) {
            if (mobs[j].hidden) continue;
            if (distance(mobs[j].x + mob_radius, mobs[j].y + mob_radius
                , sword_wave[i].x + sword_wave[i].radius, sword_wave[i].y + sword_wave[i].radius)
                <= mob_radius + sword_wave[i].radius+20) {
                al_play_sample(hitted, 2.3, 0., 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
                mobs[j].hp--;
                player_mp++;
                score += 10;
                if (mobs[j].hp <= 0) {
                    mobs[j].hidden = 1;
                    score += 50;
                }
                sword_wave[i].hidden = 1;
            }
        }
        if (sword_wave[i].hidden) continue;
        if (distance(st1_boss.boss_x + st1_boss.radius, st1_boss.boss_y + st1_boss.radius
            , sword_wave[i].x + sword_wave[i].radius, sword_wave[i].y + sword_wave[i].radius)
            <= st1_boss.radius + sword_wave[i].radius) {
            if (boss_hp > 0) {
                boss_hp -= 2;
                hitted_ind = 0;
                score += 20;
                player_mp++;
            }
            al_play_sample(hitted, 2.3, 0., 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
            sword_wave[i].hidden = 1;
        }
    }
    // 生子彈
    for (int i = 0; i < 6; ++i){
        if (freeze)
            mobs[i].att_cd = now_time;
        if (freeze || mobs[i].hidden) continue;
        if (now_time - mobs[i].att_cd >= 180){
            for (int j = 0; j < 50; ++j){
                if (mobs_bullet[j].hidden) {
                    al_play_sample(flower_sd, 100.0, 0.3, 1., ALLEGRO_PLAYMODE_ONCE, 0);
                    mobs_bullet[j].x = mobs[i].x + mob_radius;
                    mobs_bullet[j].y = mobs[i].y + mob_radius;
                    mobs_bullet[j].hidden = 0;
                    mobs_bullet[j].img = mobs[i].img_bullet;
                    mobs_bullet[j].mod = mobs[i].mod;
                    switch (mobs[i].mod) {
                    case 0:
                        mobs_bullet[j].vx = ((act_x + 20) - (mobs[i].x + mob_radius)) / 50;
                        mobs_bullet[j].vy = ((act_y + 35) - (mobs[i].y + mob_radius)) / 50;
                        break;
                    case 1:
                        mobs_bullet[j].vx = rand() % 20 - (rand()%20);
                        mobs_bullet[j].vy = -(rand() % 10 + 20);
                        break;
                    case 2:
                        mobs_bullet[j].vx = rand() % 20 - (rand() % 20);
                        mobs_bullet[j].vy = -(rand() % 20);
                        while (abs(mobs_bullet[j].vx) <= 3 || abs(mobs_bullet[j].vy) <= 3) {
                            mobs_bullet[j].vx = rand() % 20 - (rand() % 20);
                            mobs_bullet[j].vy = -(rand() % 20);
                        }
                        mobs_bullet[j].time = 0;
                        break;
                    }
                    mobs_bullet[j].hidden = 0;
                    mobs[i].att_cd = now_time + (rand()%60);
                    break;
                }
            }
        }
    }
    //draw hitted
    if(hitted_ind < 4)
        al_draw_bitmap(hitted_struck[hitted_ind], st1_boss.boss_x + 30, st1_boss.boss_y + 30, 0);
    if (!freeze && !(now_time %5))
        hitted_ind++;
    //update and draw
    for (int i = 0; i < 6; ++i) {
        if (!mobs[i].hidden)
            al_draw_bitmap(mobs[i].img, mobs[i].x, mobs[i].y, 0);
    }
    for (int i = 0; i < 4; ++i){
        if (!sword_wave[i].hidden)
            al_draw_bitmap(sword_wave[i].img, sword_wave[i].x, sword_wave[i].y, 0);
    }
    now_time = al_get_timer_count(timer);
    for (int i = 0; i < 50; ++i) {
        if (mobs_bullet[i].hidden) continue;
        if (!freeze && !(now_time%2)){
            switch (mobs_bullet[i].mod) {
            case 0:
                if (mobs_bullet[i].x <= 0 || mobs_bullet[i].x >= 2360 || mobs_bullet[i].y >= 1050 || mobs_bullet[i].y <= 490)
                    mobs_bullet[i].hidden = 1;
                else {
                    mobs_bullet[i].x += mobs_bullet[i].vx;
                    mobs_bullet[i].y += mobs_bullet[i].vy;
                    collision(mobs_bullet[i].x, mobs_bullet[i].y, mob_bullet_r);
                }
                break;
            case 1:
                if (mobs_bullet[i].x <= 0 || mobs_bullet[i].x >= 2360 || mobs_bullet[i].y >= 1050)
                    mobs_bullet[i].hidden = 1;
                else {
                    mobs_bullet[i].x += mobs_bullet[i].vx;
                    mobs_bullet[i].y += bullet_down(&mobs_bullet[i].vy);
                    collision(mobs_bullet[i].x, mobs_bullet[i].y, mob_bullet_r);
                }
                break;
            case 2:
                if (mobs_bullet[i].time >= 3)
                    mobs_bullet[i].hidden = 1;
                else {
                    mobs_bullet[i].x += mobs_bullet[i].vx;
                    mobs_bullet[i].y += mobs_bullet[i].vy;
                    if (mobs_bullet[i].y >= 1050 || mobs_bullet[i].y <= 487) {
                        mobs_bullet[i].vy = -mobs_bullet[i].vy;
                        mobs_bullet[i].y = mobs_bullet[i].y <= 487 ? 486 : 1049;
                        mobs_bullet[i].time++;
                    }
                    else if (mobs_bullet[i].x < -30 || mobs_bullet[i].x >= 2270) {
                        mobs_bullet[i].vx = -mobs_bullet[i].vx;
                        mobs_bullet[i].x = mobs_bullet[i].x < 0 ? -29 : 2269;
                        mobs_bullet[i].time++;
                    }
                    collision(mobs_bullet[i].x, mobs_bullet[i].y, mob_bullet_r);
                }
                break;
            }
        }
        al_draw_bitmap(mobs_bullet[i].img, mobs_bullet[i].x, mobs_bullet[i].y, 0);
    }
}

float bullet_down(float *v) {
    return *v += 2.1;
}

float dash(){
    dash_v = dash_v * 4.85 / 5.2;
    return dir ? dash_v : -dash_v;
}

float distance(float x1, float y1, float x2, float y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

void collision(float x, float y, float radius) {
    if (invi || freeze) return;
    if (now_time - player_got_dmg_time > 100 &&
        distance(act_x + (float)50, act_y + (float)35, x + radius, y + radius)
        < (float)radius + 15) {
        now_time = al_get_timer_count(timer);
        game_log("radius:%lf act_x+50:%d, act_y+35:%d , st1_boss.boss_x:%lf st1_boss.boss_y:%lf", st1_boss.radius, act_x + 50, act_y + 35 ,st1_boss.boss_x, st1_boss.boss_y);
        al_play_sample(got_dmg,770.0, 0., 1., ALLEGRO_PLAYMODE_ONCE, 0);
        player_hp--;
        if (player_hp <= 0)
            again();
        player_got_dmg_time = now_time;
        score -= 100;
    }
}

void flower_mem(bullet* temp) {
    int speed = rand() % 5 + 8;
    float angle = (rand() % 100) / 100. * (9*pi/10)+ 11*pi/10;
    temp->next_bullet = malloc(sizeof(bullet));
    temp->next_bullet->x = st1_boss.boss_x + st1_boss.boss_w / 2 - 45 ;
    temp->next_bullet->y = st1_boss.boss_y + st1_boss.boss_h / 2 - 45;
    temp->next_bullet->radius = boss_phase == 3 ? 25 : 20;
    temp->next_bullet->vx =  cos(angle) * speed;
    temp->next_bullet->vy = -sin(angle) * speed;
    temp->next_bullet->img = flower;
    if (!temp->next_bullet->img)
        game_abort("failed to load flower");
    temp->next_bullet->next_bullet = NULL;
    bullet_num++;
    bullet_ct++;
    al_play_sample(flower_sd, 100.0, 0.3, 1., ALLEGRO_PLAYMODE_ONCE, 0);
}

void mush_memory(falling_mush* temp) {
    temp->next_bullet = malloc(sizeof(falling_mush));
    temp->next_bullet->x = rand() % 2210;
    temp->next_bullet->y = 487;
    temp->next_bullet->radius = 60;
    temp->next_bullet->vy = rand() % 5 + 10;
    temp->next_bullet->img = st1_boss.bullet_img;
    if (!temp->next_bullet->img)
        game_abort("failed to load bullet");
    temp->next_bullet->next_bullet = NULL;
    bullet_num++;
    bullet_ct++;
}

void circle_mem(bullet* make,int tar_x,int tar_y) {
    float angle = 0;
    bullet* temp = make;
    int skip = rand() % 10+3;
    while (angle < 22) {
        /*if (abs(skip - angle) <= 2) {
            angle++;
            continue;
        }*/
        temp->next_bullet = malloc(sizeof(bullet));
        temp->next_bullet->x = (tar_x) + 1300 * cos(2. * pi / 22. * angle);
        temp->next_bullet->y = (tar_y) + 1300 * sin(2. * pi / 22. * angle);
        temp->next_bullet->radius = 50;
        temp->next_bullet->vx = (tar_x - temp->next_bullet->x) / 120;
        temp->next_bullet->vy = (tar_y - temp->next_bullet->y) / 120;
        temp->next_bullet->img = boss3_ult1_img;
        temp->next_bullet->lim_x = tar_x;
        temp->next_bullet->lim_y = tar_y;
        if (!temp->next_bullet->img)
            game_abort("failed to load bullet");
        temp->next_bullet->next_bullet = NULL;
        angle++;
        temp = temp->next_bullet;
    }
}

void circle_move(bullet* head) {
    static int last_shoot = 0;
    bullet* prev = head;
    bullet* temp = head->next_bullet;
    while (temp) {
        if (!freeze && now_time % 5) {
            temp->y += temp->vy;
            temp->x += temp->vx;
            if (distance(temp->x , temp->y , temp->lim_x,temp->lim_y)
                <= 50) {
                del_flower(prev, temp);
                temp = prev->next_bullet;
                continue;
            }
        }
        al_draw_bitmap(temp->img, temp->x, temp->y, 0);
        collision(temp->x, temp->y, temp->radius);
        prev = temp;
        temp = temp->next_bullet;
    }
    if (!freeze && now_time - last_shoot >= 90) {
        circle_mem(prev,act_x+40,act_y+25);
        last_shoot = now_time;
    }
}

void st1_make_bullet(falling_mush*head) {
    falling_mush*prev = head;
    falling_mush* temp = head->next_bullet;
    while (temp) {
        if (!freeze && now_time%2) {
            temp->y += temp->vy;
            if (temp->y >= 1050) {
                bullet_ct--;
                del_bullet(prev, temp);
                temp = prev->next_bullet;
                continue;
            }
        }
        al_draw_bitmap(temp->img, temp->x, temp->y, 0);
        collision(temp->x+7, temp->y-10, temp->radius+5);
        prev = temp;
        temp = temp->next_bullet;
    }
    while(bullet_ct < 10 && bullet_num < bullet_max)
    {
        mush_memory(prev);
        prev = prev->next_bullet;
    }
}

void flower_bullet(bullet* head) {
    bullet* prev = head;
    bullet* temp = head->next_bullet;
    while (temp) 
    {
        if (!freeze && now_time % 2) {
            switch (boss_phase) {
            case 1:
                temp->y += temp->vy;
                temp->x += temp->vx;
                if (temp->y >= 1050 || temp->x < -30 || temp->x >= 2330) {
                    bullet_ct--;
                    del_flower(prev, temp);
                    temp = prev->next_bullet;
                    continue;
                }
                break;
            case 2:
                temp->y += temp->vy;
                temp->x += temp->vx;
                if (temp->y >= 1050 || temp->y <= 487) {
                    temp->vy = -temp->vy;
                    temp->y = temp->y <= 487 ? 486 : 1049;
                }
                else if (temp->x < -30 || temp->x >= 2270) {
                    temp->vx = -temp->vx;
                    temp->x = temp->x < 0 ? -29 : 2269;
                }
                break;
            case 3:
                if (now_time - stop_bullet < 180 || now_time - stop_bullet >260) {
                    temp->y += temp->vy;
                    temp->x += temp->vx;
                    if (temp->y >= 1050 || temp->y <= 487) {
                        temp->vy = -temp->vy;
                        temp->y = temp->y <= 487 ? 486 : 1049;
                    }
                    else if (temp->x < -30 || temp->x >= 2270) {
                        temp->vx = -temp->vx;
                        temp->x = temp->x < 0 ? -29 : 2269;
                    }
                }
                else {
                        if (now_time - stop_bullet >= 220) {
                            temp->vx = rand() % 15 - (rand() % 15);
                            temp->vy = rand() % 15 - (rand() % 15);
                            flower_stop = 0;
                        }
                        else {
                            if (!flower_stop) {
                                al_play_sample(stop_sd, 3., 0., 1., ALLEGRO_PLAYMODE_ONCE, 0);
                                flower_stop = 1;
                            }
                            temp->vx = 0;
                            temp->vy = 0;
                            temp->img = stop_img;
                        }
                }
                break;
            }
        }
        al_draw_bitmap(temp->img, temp->x, temp->y, 0);
        collision(temp->x+30, temp->y+30, temp->radius);
        prev = temp;
        temp = temp->next_bullet;
    }
    if (!freeze && now_time%2 && bullet_num < bullet_max) {
            flower_mem(prev);
            prev = prev->next_bullet;
        }
}

void del_flower(bullet* prev,bullet* cur) {
    prev->next_bullet = cur->next_bullet;
    free(cur);
}

void del_bullet(falling_mush* prev, falling_mush* cur) {
    prev->next_bullet = cur->next_bullet;
    free(cur);
}

void destroy1() {
    al_destroy_bitmap(st1_boss.boss_att);
    al_destroy_bitmap(st1_boss.boss_stay);
    al_destroy_bitmap(st1_boss.boss_skill1);
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 10; ++j) {
            al_destroy_bitmap(lazer_att[i].lazer_img[j]);
        }
    }
    for(int i=0;i<4;++i)
        al_destroy_bitmap(st1_boss.boss_skill4[i]);
    for(int i=0;i<6;++i)
        al_destroy_bitmap(st1_boss.die[i]);
    al_destroy_bitmap(st1_boss.boss_skill2);
    al_destroy_bitmap(st1_boss.bullet_img);
    al_destroy_bitmap(flower);
    al_destroy_sample(st1_boss_skill2_sd);
    al_destroy_sample(BGM);
}

void destroy2() {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 10; ++j) {
            al_destroy_bitmap(lazer_att[i].lazer_img[j]);
        }
    }
    al_destroy_bitmap(st1_boss.boss_att);
    al_destroy_bitmap(st1_boss.boss_stay);
    al_destroy_bitmap(st1_boss.boss_skill1);
    al_destroy_bitmap(st1_boss.boss_skill2);
    al_destroy_bitmap(st1_boss.bullet_img);
    for (int i = 0; i < 4; ++i)
        al_destroy_bitmap(st1_boss.boss_skill4[i]);
    al_destroy_bitmap(flower);
    for (int i = 0; i < 6; ++i)
        al_destroy_bitmap(st1_boss.die[i]);
    for (int i = 0; i < 22; ++i)
        al_destroy_bitmap(boss2_ult_img[i]);
    for (int i = 0; i < 14; ++i)
        al_destroy_bitmap(boss2_ult_anime[i]);
    al_destroy_bitmap(boss2_ult_back);
    al_destroy_sample(st1_boss_skill2_sd);
    al_destroy_sample(boss_ult1_sd);
    al_destroy_sample(BGM2);
}

void destroy3() {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 10; ++j) {
            al_destroy_bitmap(lazer_att[i].lazer_img[j]);
        }
    }
    al_destroy_bitmap(st1_boss.boss_att);
    al_destroy_bitmap(st1_boss.boss_stay);
    al_destroy_bitmap(st1_boss.boss_skill1);
    al_destroy_bitmap(st1_boss.boss_skill2);
    al_destroy_bitmap(st1_boss.bullet_img);
    for (int i = 0; i < 4; ++i)
        al_destroy_bitmap(st1_boss.boss_skill4[i]);
    al_destroy_bitmap(flower);
    for (int i = 0; i < 6; ++i)
        al_destroy_bitmap(st1_boss.die[i]);
    for (int i = 0; i < 19; ++i)
        al_destroy_bitmap(boss3_sum[i]);
    al_destroy_bitmap(boss3_ult1_img);
    for (int i = 0; i < 33; ++i)
        al_destroy_bitmap(boss3_ult2_img[i]);
    al_destroy_bitmap(boss3_ult2_black);
    al_destroy_sample(stop_sd);
    al_destroy_sample(st1_boss_skill2_sd);
    al_destroy_sample(BGM3);
}

void game_abort(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    game_vlog(format, arg);
    va_end(arg);
    fprintf(stderr, "error occured, exiting after 2 secs");
    // Wait 2 secs before exiting.
    al_rest(2);
    // Force exit program.
    exit(1);
}

void game_log(const char* format, ...) {
#ifdef LOG_ENABLED
    va_list arg;
    va_start(arg, format);
    game_vlog(format, arg);
    va_end(arg);
#endif
}

void game_vlog(const char* format, va_list arg) {
#ifdef LOG_ENABLED
    static bool clear_file = true;
    vprintf(format, arg);
    printf("\n");
    // Write log to file for later debugging.
    FILE* pFile = fopen("log.txt", clear_file ? "w" : "a");
    if (pFile) {
        vfprintf(pFile, format, arg);
        fprintf(pFile, "\n");
        fclose(pFile);
    }
    clear_file = false;
#endif
}