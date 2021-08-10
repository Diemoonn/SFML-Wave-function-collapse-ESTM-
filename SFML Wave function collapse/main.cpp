#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>

using namespace sf;
using namespace std;

struct Tile
{
    string name;
    Color color;
    vector<string> compatible;
    float probability;
};

class Cell
{
public:
    vector<Tile> domain;
    Tile tile;

    Cell() {}

    Cell(Vector2f position, vector<Tile> tiles)
    {
        cell.setPosition(position);
        cell.setSize(Vector2f(16, 16));

        cell.setFillColor(color);

        for (unsigned int i = 0; i < tiles.size(); i++) // ��������� ����� ���������� ���������� ������
        {
            domain.push_back(tiles[i]);

            entropia++;
        }

        decided = false;
    }

    void decideRandom()
    {
        int randIndex, randKoef;
        float result;

        do {
            randIndex = rand() % domain.size() + 0;

            randKoef = rand() % 10 + 1;
            result = randKoef * domain[randIndex].probability;
        } while (result < 1);

        tile = domain[randIndex];
        color = tile.color;

        cell.setFillColor(color);

        decided = true;
    }

    void setTile(Tile newTile)
    {
        tile = newTile;
        color = tile.color;

        cell.setFillColor(color);
    }

    bool isDecided() const
    {
        return decided;
    }

    int getEntropia() const
    {
        return entropia;
    }

    vector<string> getCompatible() const
    {
        return tile.compatible;
    }

    void drawCell(RenderWindow &window) const
    {
        window.draw(cell);
    }

private:
    RectangleShape cell;
    int entropia = 0;
    bool decided;
    Color color = Color::Green;
};

void initCells(vector<Cell> &map, vector<Tile> tiles)
{
    // ��������� ����� �������� � ������������

    for (int y = 0; y < 256; y += 16)
    {
        for (int x = 0; x < 256; x += 16)
        {
            Cell newCell(Vector2f(x, y), tiles);

            map.push_back(newCell);
        }
    }
}

void setNewState(vector<Cell> &map, int index)
{
    // �������� ����� ��������� ��� ������

    map[index].decideRandom();
}

/*
void initTiles(int tilesetWidth, int tilesetHeight)
{
    // ������ ������ �� config-file � �� �� ������ ������ �����

    Texture tileset;
    tileset.loadFromFile("resourses/tiles textures/tileset.png");

    for (int y = 0; y < tilesetHeight; y+=16)
    {
        for (int x = 0; x < tilesetWidth; x+=16)
        {
            Tile newTile;


        }
    }
}
*/

void findNeighboors(int (&near)[4], int index)
{
    // ���������� � ����������� ������ ������� ������� � ������ ������

    int leftSide[14] = { 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224 };
    int rightSide[14] = { 31, 47, 63, 79, 95, 111, 127, 143, 159, 175, 191, 207, 223, 239 };

    // ���������, ��������� �� ������� � �������� index � �����
    switch (index)
    {
    case 0: // ����� �������
        near[0] = index + 1;
        near[1] = index + 16; // 16 � ������ ������ � ������ � ������� - ���-�� ������ �� ��������� � �����������. ��� ���������� ��������
        break;

    case 15: // ������ �������
        near[0] = index - 1;
        near[1] = index + 16;
        break;

    case 240: // ����� ������
        near[0] = index - 16;
        near[1] = index + 1;
        break;

    case 255: // ������ ������
        near[0] = index - 16;
        near[1] = index - 1;
    }

    // ���������, ��������� �� ������ � ������� ����
    if (index >= 1 && index <= 14)
    {
        near[0] = index - 1;
        near[1] = index + 1;
        near[2] = index + 16;
    }
    else if (index >= 241 && index <= 254) // ���������, ��������� �� ������ � ������ ����
    {
        near[0] = index - 1;
        near[1] = index + 1;
        near[2] = index - 16;
    }

    // ���������, ��������� �� ������ �� ����� �������
    for (int i = 0; i <= 13; i++)
    {
        if (leftSide[i] == index)
        {
            near[0] = index - 16;
            near[1] = index + 1;
            near[2] = index + 16;
        }
    }

    if (near[0] == -1)
    {
        near[0] = index - 1;
        near[1] = index + 1;
        near[2] = index - 16;
        near[3] = index + 16;
    }

    // ���������, ��������� �� ������ �� ������ �������
    for (int i = 0; i <= 13; i++)
    {
        if (rightSide[i] == index)
        {
            near[0] = index - 16;
            near[1] = index - 1;
            near[2] = index + 16;
        }
    }

    // ���������, ��������� �� ������ �� ���������� ��������
    if (near[0] == -1)
    {
        near[0] = index - 1;
        near[1] = index + 1;
        near[2] = index - 16;
        near[3] = index + 16;
    }
}

void extend(vector<Cell> &map, int index)
{
    // �������������� �����������

    /*
    �������� ��������������� �����������

    ����� ��������� �������� ������
    ��������� � �������
    ��� ������ �������� ���������� ������
        �������� ����� �������� � ��������� �������� �������
        �������� �� ������ �������� ������, �� �������� � �������� �������
    */

    int near[4] = { -1, -1, -1, -1 };

    findNeighboors(near, index);

    int neighboor;

    for (int i = 0; i < 4; i++)
    {
        if (near[i] == -1)
            continue;

        neighboor = near[i];

        vector<string>::iterator startSearch = map[index].tile.compatible.begin();
        vector<string>::iterator finishSearch = map[index].tile.compatible.end();

        if (map[neighboor].isDecided() == false) // ������������ ������ ���������� ������� �����
        {
            /*��� ������� �������� � ������ ���������� ���� ��������������� � ��������� ��������. ���� ������
            �� ����� - ������� ������� �� ������*/

            for (unsigned int i = 0; i < map[neighboor].domain.size(); i++) /*�������� �� ������ ���������� ������*/
            {
                bool found = false;

                /*����� �������� �������� �� ������ � ��������� ��������� ������.*/ 

                string searchString = map[neighboor].domain[i].name;
                vector<string>::const_iterator tileIter = find(startSearch, finishSearch, searchString);

                if (tileIter != map[index].tile.compatible.end())
                    found = true;

                if (found == false)
                    map[neighboor].domain.erase(map[neighboor].domain.begin() + i);
            }
        }
    }
}

void sandFilter(vector<Cell> &map, Tile filterTile)
{
    // �������� ����� �� �����, ���� ������, ���������� �����, �� �������� � �����

    int neighboors[4] = { -1, -1, -1, -1 };
    int currentNeighboor;
    bool hasWater;

    for (unsigned int i = 0; i < map.size(); i++) // ������� ���� �����
    {
        if (map[i].tile.name == "sand")           // ���� ����� �����
        {
            neighboors[0] = -1;
            neighboors[1] = -1;
            neighboors[2] = -1;
            neighboors[3] = -1;
            hasWater = false;

            findNeighboors(neighboors, i);        // ���� �������

            for (int j = 0; j < 4; j++)           // ������� �������
            {
                currentNeighboor = neighboors[j];

                if (currentNeighboor != -1)
                {
                    if (map[currentNeighboor].tile.name == "water") // ���� ����� - ����
                        hasWater = true;
                }
            }

            if (hasWater == false)
                map[i].setTile(filterTile);
        }
    }
}

void drawMap(RenderWindow &window, vector<Cell> map)
{
    // ������ ������� �����

    for (unsigned int i = 0; i < map.size(); i++)
        map[i].drawCell(window);
}

int main()
{
    RenderWindow window(sf::VideoMode(256, 256), "SFML Wave function collapse");

    srand(static_cast<unsigned int> (time(0)));

    Tile water, sand, ground;

    water.name = "water";
    sand.name = "sand";
    ground.name = "ground";

    water.probability = 0.5;
    sand.probability = 0.1;
    ground.probability = 0.5;

    water.color = Color::Blue;
    sand.color = Color::Yellow;
    ground.color = Color::Green;

    water.compatible.push_back("water");
    water.compatible.push_back("sand");

    sand.compatible.push_back("water");
    sand.compatible.push_back("sand");
    sand.compatible.push_back("ground");

    ground.compatible.push_back("sand");
    ground.compatible.push_back("ground");

    vector<Tile> tiles;

    tiles.push_back(water);
    tiles.push_back(sand);
    tiles.push_back(ground);

    vector<Cell> map;

    // ��������� ����������� �������� � ������������
    initCells(map, tiles);

    int lastDecided = 0;
    
    for (int i = 0; i < 256; i++)
    {
        // �������� �������������� �����������
        extend(map, lastDecided);
        
        // ����� ������ � ���������� ���������
        int minEntropia = 4;
        int minIndex = 0;
        
        for (unsigned int j = 0; j < map.size(); j++)
        {
            if ((map[j].getEntropia() < minEntropia) && (map[j].isDecided() == false))
            {
                minEntropia = map[j].getEntropia();
                minIndex = j;
            }
        }

        // ������� ������ � ���������� ���������
        setNewState(map, minIndex);
        lastDecided = minIndex;
    }

    sandFilter(map, ground);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        
        // drawing;

        drawMap(window, map);

        window.display();
    }

    return 0;
}