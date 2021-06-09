#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<stdlib.h>
#include <math.h>
#include <tchar.h>

#define U 1
#define D 2
#define L 3
#define R 4       //�ߵ�״̬��U���� ��D���£�L:�� R����

float f(float x, float y, float z) {
    float a = x * x + 9.0f / 4.0f * y * y + z * z - 1;
    return a * a * a - x * x * z * z * z - 9.0f / 80.0f * y * y * z * z * z;
}
float h(float x, float z) {
    for (float y = 1.0f; y >= 0.0f; y -= 0.001f)
        if (f(x, y, z) <= 0.0f)
            return y;
    return 0.0f;
}

typedef struct SNAKE //�����һ���ڵ�
{
    int x;
    int y;
    struct SNAKE* next;
}snake;

//ȫ�ֱ���//
int score = 0, add = 1;//�ܵ÷���ÿ�γ�ʳ��÷֡�
int status, sleeptime = 200;//ÿ�����е�ʱ����
snake* head, * food;//��ͷָ�룬ʳ��ָ��
snake* q;//�����ߵ�ʱ���õ���ָ��
int endGamestatus = 0; //��Ϸ�����������1��ײ��ǽ��2��ҧ���Լ���3�������˳���Ϸ��

//����ȫ������//
void Pos();
void creatMap();
void initSnake();
int biteSelf();
void createFood();
void cantCrossWall();
void snakeMove();
void pause();
void runGame();
void initGame();
void endGame();
void gameStart();
void Easter_Egg();

void Easter_Egg()//С����
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0c); // ��ɫ
    HANDLE o = GetStdHandle(STD_OUTPUT_HANDLE);
    _TCHAR buffer[25][80] = { _T(' ') };
    _TCHAR ramp[] = _T("*********");//.:-=+*#%@
    int count = 0;
    int count1 = 0;
    for (float t = 0.0f;; t += 0.1f) {
        int sy = 0;
        float s = sinf(t);
        float a = s * s * s * s * 0.2f;

        for (float z = 1.3f; z > -1.2f; z -= 0.1f) {
            _TCHAR* p = &buffer[sy++][0];
            float tz = z * (1.2f - a);
            for (float x = -1.5f; x < 1.5f; x += 0.05f) {
                float tx = x * (1.2f + a);
                float v = f(tx, 0.0f, tz);
                if (v <= 0.0f) {
                    float y0 = h(tx, tz);
                    float ny = 0.01f;
                    float nx = h(tx + ny, tz) - y0;
                    float nz = h(tx, tz + ny) - y0;
                    float nd = 1.0f / sqrtf(nx * nx + ny * ny + nz * nz);
                    float d = (nx + ny - nz) * nd * 0.5f + 0.5f;
                    *p++ = ramp[(int)(d * 5.0f)];
                }
                else
                    *p++ = ' ';
            }
        }

        for (sy = 0; sy < 25; sy++) {
            COORD coord = { 0, sy };
            SetConsoleCursorPosition(o, coord);
            WriteConsole(o, buffer[sy], 79, NULL, 0);
        }
        if (count <= 10) {
            //printf("\n        XXXXXXXXXXXXX\n") ;//�������I Love You ������.Mua~
            //printf("        By XXXXX");// ����ߵ�����
            count++;
        }
        else {
            printf("\n       �����ҵģ��ţ������ҵ�\n");
            printf("        By XX");
            count++;
            if (count >= 20) {
                count = 0;
            }
        }
        Sleep(33);
    }
}

void Pos(int x, int y)//���ù��λ��
{
    COORD pos;
    HANDLE hOutput;
    pos.X = x;
    pos.Y = y;
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);//���ر�׼�����롢����������豸�ľ����Ҳ���ǻ�����롢���/�������Ļ�������ľ��
    SetConsoleCursorPosition(hOutput, pos);
}

void creatMap()//������ͼ
{
    int i;
    for (i = 0; i < 58; i += 2)//��ӡ���±߿�
    {
        Pos(i, 0);
        printf("��");//һ������ռ����λ��
        Pos(i, 26);
        printf("��");
    }
    for (i = 1; i < 26; i++)//��ӡ���ұ߿�
    {
        Pos(0, i);
        printf("��");
        Pos(56, i);
        printf("��");
    }
}

void initSnake()//��ʼ������
{
    snake* tail;
    int i;
    tail = (snake*)malloc(sizeof(snake));//����β��ʼ��ͷ�巨����x,y�趨��ʼ��λ��//
    tail->x = 24;
    tail->y = 5;
    tail->next = NULL;
    for (i = 1; i <= 4; i++)//��ʼ����Ϊ4
    {
        head = (snake*)malloc(sizeof(snake));
        head->next = tail;
        head->x = 24 + 2 * i;
        head->y = 5;
        tail = head;
    }
    while (tail != NULL)//��ͷ��Ϊ���������
    {
        Pos(tail->x, tail->y);
        printf("��");
        tail = tail->next;
    }
}
//??
int biteSelf()//�ж��Ƿ�ҧ�����Լ�
{
    snake* self;
    self = head->next;
    while (self != NULL)
    {
        if (self->x == head->x && self->y == head->y)
        {
            return 1;
        }
        self = self->next;
    }
    return 0;
}

void createFood()//�������ʳ��
{
    snake* food_1;
    srand((unsigned)time(NULL));//Ϊ�˷�ֹÿ�β������������ͬ����������Ϊtime
    food_1 = (snake*)malloc(sizeof(snake));
    while ((food_1->x % 2) != 0)    //��֤��Ϊż����ʹ��ʳ��������ͷ����
    {
        food_1->x = rand() % 52 + 2;
    }
    food_1->y = rand() % 24 + 1;
    q = head;
    while (q->next == NULL)
    {
        if (q->x == food_1->x && q->y == food_1->y) //�ж������Ƿ���ʳ���غ�
        {
            free(food_1);
            createFood();
        }
        q = q->next;
    }
    Pos(food_1->x, food_1->y);
    food = food_1;
    printf("��");
}

void cantCrossWall()//���ܴ�ǽ
{
    if (head->x == 0 || head->x == 56 || head->y == 0 || head->y == 26)
    {
        endGamestatus = 1;
        endGame();
    }
}

void snakeMove()//��ǰ��,��U,��D,��L,��R
{
    snake* nexthead;
    cantCrossWall();

    nexthead = (snake*)malloc(sizeof(snake));
    if (status == U)
    {
        nexthead->x = head->x;
        nexthead->y = head->y - 1;
        if (nexthead->x == food->x && nexthead->y == food->y)//�����һ����ʳ��//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            score = score + add;
            createFood();
        }
        else                                               //���û��ʳ��//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == D)
    {
        nexthead->x = head->x;
        nexthead->y = head->y + 1;
        if (nexthead->x == food->x && nexthead->y == food->y)  //��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            score = score + add;
            createFood();
        }
        else                               //û��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == L)
    {
        nexthead->x = head->x - 2;
        nexthead->y = head->y;
        if (nexthead->x == food->x && nexthead->y == food->y)//��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            score = score + add;
            createFood();
        }
        else                                //û��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == R)
    {
        nexthead->x = head->x + 2;
        nexthead->y = head->y;
        if (nexthead->x == food->x && nexthead->y == food->y)//��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            score = score + add;
            createFood();
        }
        else                                         //û��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (biteSelf() == 1)       //�ж��Ƿ��ҧ���Լ�
    {
        endGamestatus = 2;
        endGame();
    }
}

void pause()//��ͣ
{
    while (1)
    {
        Sleep(300);
        if (GetAsyncKeyState(VK_SPACE))
        {
            break;
        }

    }
}

void runGame()//������Ϸ
{

    Pos(64, 15);
    printf("���Ų��ܴ�ǽ������ҧ���Լ�Ŷ\n");
    Pos(64, 16);
    printf("�á�.��.��.���ֱ�����ߵ��ƶ�.");
    Pos(64, 17);
    printf("F1 Ϊ���٣�F2 Ϊ����\n");
    Pos(64, 18);
    printf("ESC ���˳���Ϸ.space����ͣ��Ϸ.");
    Pos(64, 20);
    status = R;
    while (1)
    {
        Pos(64, 7);
        printf("�÷֣�%d  ", score);
        Pos(64, 8);
        printf("ÿ��ʳ��÷֣�%d��", add);
        Pos(64, 10);

        if (score >= 120)
        {
            endGamestatus = 4;
            break;
        }
        else if (GetAsyncKeyState(VK_UP) && status != D)
        {
            status = U;
        }
        else if (GetAsyncKeyState(VK_DOWN) && status != U)
        {
            status = D;
        }
        else if (GetAsyncKeyState(VK_LEFT) && status != R)
        {
            status = L;
        }
        else if (GetAsyncKeyState(VK_RIGHT) && status != L)
        {
            status = R;
        }
        else if (GetAsyncKeyState(VK_SPACE))
        {
            pause();
        }
        else if (GetAsyncKeyState(VK_ESCAPE))
        {
            endGamestatus = 3;
            break;
        }
        else if (GetAsyncKeyState(VK_F1))
        {
            if (sleeptime >= 50)
            {
                sleeptime = sleeptime - 30;
                add = add + 2;
                if (sleeptime == 320)
                {
                    add = 2;//��ֹ����1֮���ټӻ����д�
                }
            }
        }
        else if (GetAsyncKeyState(VK_F2))
        {
            if (sleeptime < 350)
            {
                sleeptime = sleeptime + 30;
                add = add - 2;
                if (sleeptime == 350)
                {
                    add = 1;  //��֤��ͷ�Ϊ1
                }
            }
        }
        Sleep(sleeptime);
        snakeMove();
    }
}

void initGame()//��ʼ����
{
    Pos(40, 12);
    system("title ");
    printf("��ӭ����̰ʳ����Ϸ��");
    Pos(40, 25);
    system("pause");
    system("cls");
    Pos(25, 12);
    printf("�á�.��.��.���ֱ�����ߵ��ƶ��� F1 Ϊ���٣�2 Ϊ����\n");
    Pos(25, 13);
    printf("���ٽ��ܵõ����ߵķ�����\n");
    system("pause");
    system("cls");
}

void endGame()//������Ϸ
{

    system("cls");
    Pos(24, 12);
    if (endGamestatus == 1)
    {
        printf("С��������ײ��ǽ��Ŷ\n\n");
        Pos(24, 14);
    }
    else if (endGamestatus == 2)
    {
        printf("��Ϸ����.\n");
    }
    else if (endGamestatus == 3)
    {
        printf("���Ѿ���������Ϸ\n");
    }
    else if (endGamestatus == 4)
    {
        printf("�������زʵ���\n");
        Sleep(1000);
        Easter_Egg();
    }
    Pos(24, 13);
    printf("���ĵ÷���%d\n ", score);
    while (getchar() != 'y')
    {
        printf("close?[y]");
    }
    exit(0);
}

void gameStart()//��Ϸ��ʼ��
{
    system("mode con cols=100 lines=30");
    initGame();
    creatMap();
    initSnake();
    createFood();
}


void main()
{
     gameStart();
     runGame();
     endGame();
}