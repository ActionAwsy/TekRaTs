#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_native_dialog.h>
#include "Objetos.h"

//------------------------------ GLOBAIS ---------------------------//

const int LARGURA = 800;
const int ALTURA = 600;
const int FPS = 60;
const int NUM_BALAS = 5;
const int NUM_INIMIGOS = 20;
enum TECLAS { CIMA, BAIXO, ESQUERDA, DIREITA, SPACE, ENTER };
bool teclas[6] = { false, false, false, false, false, false };

//------------------------------ OBJETOS ---------------------------//

struct Background {
    float x;
    float dirX;
    float velX;
    int width;
    int height;

    ALLEGRO_BITMAP* imagem;
};

//------------------------------ PROTÓTIPOS ---------------------------//

void InitPerso(Personagem& perso);

void MovePersoCima(Personagem& perso);
void MovePersoBaixo(Personagem& perso);
void MovePersoDireita(Personagem& perso);
void MovePersoEsquerda(Personagem& perso);

void InitBalas(Projeteis balas[], int tamanho);
void AtiraBalas(Projeteis balas[], int tamanho, Personagem perso);
void AtualizaBalas(Projeteis balas[], int tamanho);
void DesenharBalas(Projeteis balas[], int tamanho);
void DesenharBalaHS(Projeteis balas[], int tamanho);
void BalaColidida(Projeteis balas[], int b_tamanho, Inimigo inimigos[], int c_tamanho, Personagem& perso);
void BalaColididaHS(Projeteis balas[], int b_tamanho, Inimigo inimigos[], int c_tamanho, Personagem& perso);

void InitInimigo(Inimigo inim[], int tamanho);
void LiberaInimigo(Inimigo inim[], int tamanho);
void AtualizarInimigo(Inimigo inim[], int tamanho);
void DesenhaInimigo(Inimigo inim[], int tamanho);
void InimigoColidido(Inimigo inim[], int c_tamanho, Personagem& perso);

void InitBackground(Background& back, float x, float velX, float dirX, float width, float height, ALLEGRO_BITMAP* imagem);
void UpdBackground(Background& back);
void DesenBackground(Background& back);

int main()
{
    //------------------------------ VARIÁVEIS PRIMITIVAS ---------------------------//

    bool fim = false;
    bool desenha = true;
    bool game_over = false;

    Background BG;

    //------------------------------ VARIÁVEIS DE OBJETOS ---------------------------//

    Personagem perso;
    Inimigo inim[NUM_INIMIGOS];
    Projeteis balas[NUM_BALAS];

    //------------------------------ VARIÁVEIS DO ALLEGRO ---------------------------//

    ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;
    ALLEGRO_TIMER* timer = NULL;
    ALLEGRO_BITMAP* enemy = NULL;
    ALLEGRO_BITMAP* player = NULL;
    ALLEGRO_BITMAP* background = NULL;
    ALLEGRO_SAMPLE* trilha_sonora = NULL;
    ALLEGRO_SAMPLE_INSTANCE* inst_trilha = NULL;
    ALLEGRO_FONT* font14 = NULL;

    //------------------------------- INICIALIZAÇÃO -----------------------------------//

    ALLEGRO_DISPLAY* display = NULL;

    if (!al_init()) {
        al_show_native_message_box(NULL, "AVISO", "ERRO", "INICIALIZAR DO ALLEGRO", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    display = al_create_display(LARGURA, ALTURA);

    if (!display) {
        al_show_native_message_box(NULL, "AVISO", "ERRO", "CRIAR DISPLAY", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    //------------------------------ INICIALIZAÇÃO DE ADDONS ---------------------------//

    al_init_primitives_addon();
    al_install_keyboard();
    al_init_image_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(10);
    al_init_font_addon();
    al_init_ttf_addon();

    //---------------------------------- CRIAÇÃO DA FILA --------------------------------//

    fila_eventos = al_create_event_queue();
    timer = al_create_timer(1.0 / FPS);
    font14 = al_load_font("arial.ttf", 14, NULL);   //Fonte das Letras

    //------------------------------ CRIAÇÃO DE DISPOSITIVOS --------------------------//    

    background = al_load_bitmap("imgs/Bgplaceholder.png");                             //Aqui que muda a imagem de fundo//
    InitBackground(BG, 0, 5, -1, 800, 600, background);
    al_flip_display();

    player = al_load_bitmap("imgs/Richardplaceholder.png");                            //Aqui que muda a imagem do personagem//
    enemy = al_load_bitmap("imgs/Enemyplaceholder.png");
    fila_eventos = al_create_event_queue();
    trilha_sonora = al_load_sample("audio/musica.ogg");                                //Aqui muda a trilha sonora//
    inst_trilha = al_create_sample_instance(trilha_sonora);
    al_attach_sample_instance_to_mixer(inst_trilha, al_get_default_mixer());
    al_set_sample_instance_playmode(inst_trilha, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_gain(inst_trilha, 0.8);

    //------------------------- FUNÇÕES INICIAIS --------------------------------//

    srand(time(NULL));

    InitPerso(perso);
    InitInimigo(inim, NUM_INIMIGOS);
    InitBalas(balas, NUM_BALAS);

    //--------------------------- REGSITRO DE SOURCES -----------------------------//

    al_register_event_source(fila_eventos, al_get_display_event_source(display));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

    //------------------------------ LOOP -------------------------------------//

    al_start_timer(timer);

    while (!fim) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(fila_eventos, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            fim = true;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {

            al_play_sample_instance(inst_trilha);

            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_ESCAPE:
                fim = true;
                break;

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

            case ALLEGRO_KEY_SPACE:
                teclas[SPACE] = true;
                AtiraBalas(balas, NUM_BALAS, perso);
                break;

            case ALLEGRO_KEY_ENTER:
                teclas[ENTER] = true;
                break;
            }
        }

        else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                teclas[CIMA] = false;
                break;

            case ALLEGRO_KEY_DOWN:
                teclas[BAIXO] = false;
                break;

            case ALLEGRO_KEY_RIGHT:
                teclas[DIREITA] = false;
                break;

            case ALLEGRO_KEY_LEFT:
                teclas[ESQUERDA] = false;
                break;

            case ALLEGRO_KEY_ENTER:
                teclas[ENTER] = false;
                break;
            }
        }

        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            desenha = true;
            UpdBackground(BG);

            if (teclas[CIMA]) {
                MovePersoCima(perso);
            }
            if (teclas[BAIXO]) {
                MovePersoBaixo(perso);
            }
            if (teclas[DIREITA]) {
                MovePersoDireita(perso);
            }
            if (teclas[ESQUERDA]) {
                MovePersoEsquerda(perso);
            }
            if (teclas[SPACE]) {
                AtualizaBalas(balas, NUM_BALAS);
            }
            if (!game_over) {
                LiberaInimigo(inim, NUM_INIMIGOS);
                AtualizarInimigo(inim, NUM_INIMIGOS);
                BalaColidida(balas, NUM_BALAS, inim, NUM_INIMIGOS, perso);
                InimigoColidido(inim, NUM_INIMIGOS, perso);

                if (perso.vidas <= 0) {
                    game_over = true;
                }
            }
        }

        //----------------------------- DESENHO -----------------------------------//

        if (desenha && al_is_event_queue_empty(fila_eventos)) {
            desenha = false;

            if (!game_over) {
                DesenBackground(BG);

                    if (NUM_BALAS >= 2)
                    {
                        DesenharBalaHS(balas, NUM_BALAS);
                    }
                    else
                    {
                        DesenharBalas(balas, NUM_BALAS);
                    }

                DesenhaInimigo(inim, NUM_INIMIGOS);

                al_draw_textf(font14, al_map_rgb(255, 255, 255), 0, 0, NULL, "VIDAS: %d  /  Pontos: %d", perso.vidas, perso.pontos);


            }
            else if (game_over) {
                al_draw_textf(font14, al_map_rgb(255, 255, 255), LARGURA / 2, ALTURA / 2, ALLEGRO_ALIGN_CENTER, "Fim de Jogo.  Seus Pontos: %d.  Pressione ENTER Para Jogar Novamente ou ESC Para Sair", perso.pontos);
                teclas[CIMA] = false;
                teclas[BAIXO] = false;
                teclas[DIREITA] = false;
                teclas[ESQUERDA] = false;
                perso.x = 800;

                if (teclas[ENTER]) {
                    InitPerso(perso);
                    InitInimigo(inim, NUM_INIMIGOS);
                    InitBalas(balas, NUM_BALAS);

                    game_over = false;
                }
            }
            al_draw_bitmap(player, perso.x, perso.y, NULL);
            al_flip_display();
            al_clear_to_color(al_map_rgb(0, 0, 0));
        }
    }

    //----------------------- FINALIZAÇÃO --------------------------------//

    al_destroy_display(display);
    al_destroy_event_queue(fila_eventos);
    al_destroy_timer(timer);
    al_destroy_sample(trilha_sonora);
    al_destroy_sample_instance(inst_trilha);
    al_destroy_bitmap(player);
    al_destroy_bitmap(background);
    al_destroy_font(font14);

    return 0;
}

void InitPerso(Personagem& perso)
{
    perso.x = 20;
    perso.y = 300;
    perso.vidas = 3;
    perso.velocidade = 7;
    perso.borda_x = 40;          //Ajusta Posição Vertical da Hitbox
    perso.borda_y = 40;         //Ajusta Posição Horizontal da Hitbox
    perso.pontos = 0;

    al_draw_filled_rectangle(perso.x - perso.borda_x, perso.y - perso.borda_y, perso.x + perso.borda_x, perso.y + perso.borda_y, al_map_rgb(255, 0, 255));
}

void MovePersoCima(Personagem& perso)
{
    perso.y -= perso.velocidade;

    if (perso.y < 280) {
        perso.y = 280;
    }
}

void MovePersoBaixo(Personagem& perso)
{
    perso.y += perso.velocidade;

    if (perso.y > 480) {
        perso.y = 480;
    }
}

void MovePersoDireita(Personagem& perso)
{
    perso.x += perso.velocidade;

    if (perso.x > LARGURA / 4) {
        perso.x = LARGURA / 4;
    }
}

void MovePersoEsquerda(Personagem& perso)
{
    perso.x -= perso.velocidade;

    if (perso.x < 0) {
        perso.x = 0;
    }
}

void InitBalas(Projeteis balas[], int tamanho)
{
    for (int i = 0; i < tamanho; i++) {
        balas[i].velocidade = 10;
        balas[i].ativo = false;
    }
}

void AtiraBalas(Projeteis balas[], int tamanho, Personagem perso)
{
    for (int i = 0; i < tamanho; i++) {
        if (!balas[i].ativo) {
            balas[i].x = perso.x + 30;        //Posição horizontal da bala//
            balas[i].y = perso.y + 60;        //Posição vertical da Bala//
            balas[i].ativo = true;
            break;
        }
    }
}

void AtualizaBalas(Projeteis balas[], int tamanho)
{
    for (int i = 0; i < tamanho; i++) {
        if (balas[i].ativo) {
            balas[i].x += balas[i].velocidade;

            if (balas[i].x > LARGURA) {
                balas[i].ativo = false;
            }
        }
    }
}

void DesenharBalas(Projeteis balas[], int tamanho)
{
    for (int i = 0; i < tamanho; i++) {
        if (balas[i].ativo) {
            al_draw_filled_ellipse(balas[i].x, balas[i].y, 16, 1, al_map_rgb(255, 200, 0));
        }
    }
}

void DesenharBalaHS(Projeteis balas[], int tamanho)
{
    for (int i = 0; i < tamanho; i++) {
        if (balas[i].ativo) {
            al_draw_filled_ellipse(balas[i].x, balas[i].y, 20, 2, al_map_rgb(255, 200, 0));
        }
    }
}

void BalaColidida(Projeteis balas[], int b_tamanho, Inimigo inim[], int c_tamanho, Personagem& perso)
{
    for (int i = 0; i < b_tamanho; i++)
    {
        if (balas[i].ativo)
        {
            for (int j = 0; j < c_tamanho; j++)
            {
                if (inim[j].ativo)
                {
                    if (balas[i].x > (inim[j].x - inim[j].borda_x) &&
                        balas[i].x < (inim[j].x + inim[j].borda_x) &&
                        balas[i].y >(inim[j].y - inim[j].borda_y) &&
                        balas[i].y < (inim[j].y + inim[j].borda_y))
                    {
                        balas[i].ativo = false;
                        inim[j].ativo = false;
                        perso.pontos++;
                        inim[i].velocidade++;
                    }
                }
            }
        }
    }
}

void BalaColididaHS(Projeteis balas[], int b_tamanho, Inimigo inim[], int c_tamanho, Personagem& perso)
{
    for (int i = 0; i < b_tamanho; i++)
    {
        if (balas[i].ativo)
        {
            for (int j = 0; j < c_tamanho; j++)
            {
                if (inim[j].ativo)
                {
                    if (balas[i].x > (inim[j].x - inim[j].borda_x) &&
                        balas[i].x < (inim[j].x + inim[j].borda_x) &&
                        balas[i].y >(inim[j].y - inim[j].borda_y) &&
                        balas[i].y < (inim[j].y + inim[j].borda_y))
                    {
                        balas[i].ativo = true;
                        inim[j].ativo = false;
                        perso.pontos++;
                        inim[i].velocidade++;
                    }
                }
            }
        }
    }
}

void InitInimigo(Inimigo inim[], int tamanho)
{
    for (int i = 0; i < tamanho; i++)
    {
        inim[i].ID = Inim;
        inim[i].velocidade = 3;
        inim[i].borda_x = 18;
        inim[i].borda_y = 18;
        inim[i].ativo = false;
    }
}

void LiberaInimigo(Inimigo inim[], int tamanho)
{
    for (int i = 0; i < tamanho; i++)
    {
        if (!inim[i].ativo)
        {
            if (rand() % 1000 == 0)
            {
                inim[i].x = LARGURA;
                srand(time(NULL));
                int j = rand() % 3;
                switch (j)
                {
                case 0:
                    inim[i].y = 350;
                    break;
                case 1:
                    inim[i].y = 425;
                    break;
                case 2:
                    inim[i].y = 500;
                    break;
                }
                inim[i].ativo = true;
                break;
            }
        }
    }
}

void AtualizarInimigo(Inimigo inim[], int tamanho)
{
    for (int i = 0; i < tamanho; i++)
    {
        if (inim[i].ativo)
        {
            inim[i].x -= inim[i].velocidade;

        }
    }
}

void DesenhaInimigo(Inimigo inim[], int tamanho)
{
    for (int i = 0; i < tamanho; i++)
    {
        if (inim[i].ativo)
        {
            al_draw_filled_circle(inim[i].x, inim[i].y, 20, al_map_rgb(255, 0, 0));
        }
    }
}

void InimigoColidido(Inimigo inim[], int c_tamanho, Personagem& perso)
{
    for (int i = 0; i < c_tamanho; i++)
    {
        if (inim[i].ativo)
        {
            if ((inim[i].x - inim[i].borda_x) < (perso.x + perso.borda_x) &&
                (inim[i].x + inim[i].borda_x) > (perso.x - perso.borda_x) &&
                (inim[i].y - inim[i].borda_y) < (perso.y + perso.borda_y) &&
                (inim[i].y + inim[i].borda_y) > (perso.y - perso.borda_y))
            {
                inim[i].ativo = false;
                perso.vidas--;
            }
            else if (inim[i].x < 0)
            {
                inim[i].ativo = false;
                perso.vidas--;
            }
        }
    }

}

void InitBackground(Background& back, float x, float velX, float dirX, float width, float height, ALLEGRO_BITMAP* imagem)
{
    back.x = x;
    back.velX = velX;
    back.dirX = dirX;
    back.width = width;
    back.height = height;
    back.imagem = imagem;
}

void UpdBackground(Background& back) {
    back.x += back.velX * back.dirX;
    if (back.x + back.width <= 0)
        back.x = 0;
}

void DesenBackground(Background& back) {
    al_draw_bitmap(back.imagem, back.x, 0, 0);

    if (back.x + back.width < LARGURA)
        al_draw_bitmap(back.imagem, back.x + back.width, 0, 0);
}