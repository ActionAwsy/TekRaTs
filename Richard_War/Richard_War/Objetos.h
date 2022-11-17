//IDs de Objeto
enum IDS { Perso, Balas, Inim };

//Nosso Player
struct Personagem {
    int ID;
    int x;
    int y;
    int vidas;
    int velocidade;
    int borda_x;
    int borda_y;
    int pontos;
};

struct Projeteis {
    int ID;
    int x;
    int y;
    int velocidade;
    bool ativo;
};

struct Inimigo {
    int ID;
    int x;
    int y;
    bool ativo;
    int velocidade;
    int borda_x;
    int borda_y;
};