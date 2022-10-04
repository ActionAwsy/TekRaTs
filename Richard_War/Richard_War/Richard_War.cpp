#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>

const int LARGURA = 640;
const int ALTURA = 490;
enum TECLAS { CIMA, BAIXO, ESQUERDA, DIREITA };

int main()
{
    bool fim = false;
    bool teclas[] = { false, false, false, false };
    int pos_x = 50;                //Aqui define a posiçao horizontal que o boneco vai spawnar//
    int pos_y = 215;                //Aqui define a posiçao vertical que o boneco vai spawnar//

    //--------- INICIALIZAÇAO --------//
    ALLEGRO_DISPLAY* display = NULL;
    ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;
    ALLEGRO_BITMAP* player = NULL;
    ALLEGRO_BITMAP* background = NULL;
    ALLEGRO_SAMPLE* trilha_sonora = NULL;
    ALLEGRO_SAMPLE_INSTANCE* inst_trilha = NULL;

    if (!al_init())
    {
        fprintf(stderr, "Falha ao inicializar a Allegro.\n");
        return -1;
    }

    display = al_create_display(LARGURA, ALTURA);

    if (!display)
    {
        fprintf(stderr, "Falha ao criar janela.\n");
        return -1;
    }

    //------ INICIALIZAÇAO DE ADDONS --------//
    al_install_keyboard();
    al_init_image_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(10);
    //------ CRIAÇAO DA DISPOSITIVOS --------//
    background = al_load_bitmap("imgs/Bgplaceholder.png");               //Aqui que muda a imagem de fundo//
    al_draw_bitmap(background, 0, 0, 0);
    al_flip_display();

    player = al_load_bitmap("imgs/Richardplaceholder.bmp");                   //Aqui que muda a imagem do personagem//
    fila_eventos = al_create_event_queue();
    trilha_sonora = al_load_sample("audio/musica.ogg");         //Aqui muda a trilha sonora//
    inst_trilha = al_create_sample_instance(trilha_sonora);
    al_attach_sample_instance_to_mixer(inst_trilha, al_get_default_mixer());
    al_set_sample_instance_playmode(inst_trilha, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_gain(inst_trilha, 0.8);

    //------- REGISTRO DE SOURCES ----------//
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(display));
    //-------------- LOOP -----------------//
    while (!fim)
    {
        ALLEGRO_EVENT ev;


        al_wait_for_event(fila_eventos, &ev);
        al_draw_bitmap(background, 0, 0, 0);
        al_flip_display();

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            al_play_sample_instance(inst_trilha);
            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                fim = true;
            }

            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                teclas[CIMA] = true;
                break;

            case ALLEGRO_KEY_DOWN:
                teclas[BAIXO] = true;
                break;

            case ALLEGRO_KEY_RIGHT:
                teclas[DIREITA] = true;
                break;

            case ALLEGRO_KEY_LEFT:
                teclas[ESQUERDA] = true;
                break;
            }
        }

        if (ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                teclas[CIMA] = false;
                break;

            case ALLEGRO_KEY_DOWN:
                teclas[BAIXO] = false;
                break;

            case ALLEGRO_KEY_LEFT:
                teclas[ESQUERDA] = false;
                break;

            case ALLEGRO_KEY_RIGHT:
                teclas[DIREITA] = false;
                break;
            }
        }


        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            fim = true;
        }
        if (pos_y < 250) {             
            pos_y -= teclas[CIMA] * 4;
            pos_y += teclas[BAIXO] * 4;
            pos_x -= teclas[ESQUERDA] * 4;
            pos_x += teclas[DIREITA] * 4;
        }
        else {
            pos_y = 240;               
        }

        if (pos_y > 185) {             
            pos_y -= teclas[CIMA] * 4;
            pos_y += teclas[BAIXO] * 4;
            pos_x -= teclas[ESQUERDA] * 4;
            pos_x += teclas[DIREITA] * 4;
        }
        else {
            pos_y = 190;                
        }

        if (pos_x > 9) {                
            pos_y -= teclas[CIMA] * 4;
            pos_y += teclas[BAIXO] * 4;
            pos_x -= teclas[ESQUERDA] * 4;
            pos_x += teclas[DIREITA] * 4;
        }
        else {
            pos_x = 10;                 
        }

        if (pos_x < 25) {              
            pos_y -= teclas[CIMA] * 4;
            pos_y += teclas[BAIXO] * 4;
            pos_x -= teclas[ESQUERDA] * 4;
            pos_x += teclas[DIREITA] * 4;
        }
        else {
            pos_x = 30;                
        }
        //------------- DESENHO --------------//
        al_draw_bitmap(player, pos_x, pos_y, NULL);
        al_flip_display();
        al_clear_to_color(al_map_rgb(0, 0, 0));
    }

    //---------- FINALIZAÇOES -----------//
    al_destroy_display(display);
    al_destroy_bitmap(player);
    al_destroy_event_queue(fila_eventos);
    al_destroy_sample(trilha_sonora);
    al_destroy_sample_instance(inst_trilha);

    return 0;
}