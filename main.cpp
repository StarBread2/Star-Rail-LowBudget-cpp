#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <conio.h>

//Additionals for character config (json)
#include <fstream>
#include <filesystem>
#include "include/json.hpp"
using json = nlohmann::json;

using namespace std;

class Animation
{
public:
    Animation(sf::Texture* Texture, sf::Vector2u ImageCount, float SwitchTime);
    ~Animation();

    void UpdateGrid(int Row, float DeltaTime, sf::Vector2u FrameLimit);
    void UpdateGridAfterUltimate(int Row, float DeltaTime, sf::Vector2u FrameLimit);
    void UpdateHorizontal(int Row, float DeltaTime);
    void AnimationReset(sf::Vector2u &ImageLimitBoth);

public:
    sf::IntRect UvRect;
    sf::Vector2u CurrentImage;

private:
    sf::Vector2u ImageCount;

    float TotalTime;
    float SwitchTime;
};
Animation::Animation(sf::Texture* Texture, sf::Vector2u ImageCount, float SwitchTime)
{
    this->ImageCount = ImageCount;
    this->SwitchTime = SwitchTime;
    TotalTime = 0.0f;
    CurrentImage.x = 0;

    UvRect.width = Texture->getSize().x / float(ImageCount.x);
    UvRect.height = Texture->getSize().y / float(ImageCount.y);
}
Animation::~Animation()
{

}


void Animation::UpdateGrid(int Row, float DeltaTime, sf::Vector2u FrameLimit)//NEW FUNCTION FOR THIS ANIMATION UVRECT ISSUE
{
    TotalTime += DeltaTime;

    if (CurrentImage.y != FrameLimit.y)
    {
        if (TotalTime >= SwitchTime)
        {
            TotalTime -= SwitchTime;
            CurrentImage.x++;

            if (CurrentImage.x >= ImageCount.x)
            {
                CurrentImage.x = 0;
                CurrentImage.y++;

                if (CurrentImage.y >= ImageCount.y)
                {
                    CurrentImage.y = 0;
                }
            }

        }
    }
    UvRect.left = CurrentImage.x * UvRect.width;
    UvRect.top = CurrentImage.y * UvRect.height;

    if (CurrentImage.y == FrameLimit.y)
    {
        if (CurrentImage.x <= FrameLimit.x)
        {
            CurrentImage.x++;
            if (CurrentImage.x >= FrameLimit.x)
            {
                CurrentImage.x = 0;
                CurrentImage.y = 0;
            }
        }
    }

}
void Animation::UpdateGridAfterUltimate(int Row, float DeltaTime, sf::Vector2u FrameLimit)//walay pulos ang rowend
{
    TotalTime += DeltaTime;

    if (CurrentImage.y != FrameLimit.y)
    {
        if (TotalTime >= SwitchTime)
        {
            TotalTime -= SwitchTime;
            CurrentImage.x++;

            if (CurrentImage.x >= ImageCount.x)
            {
                CurrentImage.x = 0;
                CurrentImage.y++;

                if (CurrentImage.y >= ImageCount.y)
                {
                    CurrentImage.y = 0;
                }
            }

        }
    }
    if (CurrentImage.y == FrameLimit.y)
    {
        if (CurrentImage.x <= FrameLimit.x)
        {
            CurrentImage.x++;

            if (CurrentImage.x >= FrameLimit.x)
            {
                CurrentImage.x = 0;
                CurrentImage.y = 0;
            }
        }
    }
    UvRect.left = CurrentImage.x * UvRect.width;
    UvRect.top = CurrentImage.y * UvRect.height;
}
void Animation::UpdateHorizontal(int Row, float DeltaTime)
{
    CurrentImage.y = Row;
    TotalTime += DeltaTime;

    if (TotalTime >= SwitchTime)
    {
        TotalTime -= SwitchTime;
        CurrentImage.x++;

        if (CurrentImage.x >= ImageCount.x)
        {
            CurrentImage.x = 0;
        }
    }
    UvRect.left = CurrentImage.x * UvRect.width;
    UvRect.top = CurrentImage.y * UvRect.height;
}
void Animation::AnimationReset(sf::Vector2u &ImageLimitBoth)
{
    //cout << "Function Executed" << endl;
    //cout << "Function Above X: " << ImageLimitBoth.x << "Function Above Y: " << ImageLimitBoth.y << endl;
    ImageLimitBoth.y = 0;
    ImageLimitBoth.x = 0;
    //cout << "Below Function X: " << ImageLimitBoth.x << "Below Function Y: " << ImageLimitBoth.y << endl;
}

class Character
{
public:
    string Name;
    int Speed;
    int ActionValue;
    int Health;
    int Attack;
    bool Player;
    float ERRate;
    float EnergyInside;
    float CritRate;
    float CritDamage;
    bool IsDead;
    int IsEnemy1;
    int WhatCharacter; //0 = Furina   1 = Nigga Cat  2= Chonker Cat

    Character(string n, int s, int h, int a, bool p, float e, float ei, float c, float cd, float id, int e1, int wc)
        : Name(n), Speed(s), ActionValue(0), Health(h), Attack(a), Player(p), ERRate(e), EnergyInside(ei), CritRate(c), CritDamage(cd), IsDead(id), IsEnemy1(e1), WhatCharacter(wc) {}
    
    //Additionals
    Character()
    {
        Name = "";
        Speed = 0;
        ActionValue = 0;
        Health = 0;
        Attack = 0;
        Player = false;
        ERRate = 0;
        EnergyInside = 0;
        CritRate = 0;
        CritDamage = 0;
        IsDead = false;
        IsEnemy1 = 0;
        WhatCharacter = 0;
    }
};

int CalculateActionValue(int ActionGauge, int Speed);
double CalculateBaseDamage(double DamagePercentage, double ScalingAttribute); //scalingAttribute = attack
double EnergyCalculator(double EnergyRegenerationRate, int BaseEnergy);
void ActionValueSorter(vector<Character>& characters);
void DisplayUI(int SkillPoint, float CharacterEnergy);
int ApplyDamageAndLimiter(int CharacterHealth, int Damage, bool& IsDead);
float AddAndDisplayEnergyPointAndLimiter(float MainCharacterEnergy, float MainCharacterEnergyRate, int EnergyRecharge);
void DisplayDamageDealt(string FirstCharacterName, int FirstCharacterDamage, string SecondCharacterName, int SecondCharacterHealth, bool& IfCrit);
int AddAndDisplaySkillPointAndLimiter(int SkillPoint);
int SubtractAndDisplaySkillPointAndLimiter(int SkillPoint);
bool CritIdentifier(float CritRate);

int main()
{
    #pragma region //Additionals (Get character config data)
        std::string path = std::filesystem::current_path().string() + "/config.json";

        std::ifstream file(path);

        if (!file.is_open())
        {
            std::cout << "Failed to open config.json at:\n" << path << std::endl;
        }

        json config;
        file >> config;

        std::cout << "Config loaded successfully!" << std::endl;
    #pragma endregion
    #pragma region // Load Textures
        sf::RenderWindow Window(sf::VideoMode(1600, 900), "SFML works!");
        Window.setFramerateLimit(60);

        cout << "Loading Textures 1: ";
        //BASIC ATTACK ANIMATIONS
        sf::RectangleShape MainBackground_BasicAttack(sf::Vector2f(1600.0f, 900.0f));//Have Animations
        sf::RectangleShape Furina_Shape_BasicAttack(sf::Vector2f(1600.0f, 900.0f));
        sf::RectangleShape EnemyCharacter1_BasicAttack(sf::Vector2f(1600.0f, 900.0f)); //BlackPusa Have Animations
        sf::RectangleShape EnemyCharacter2_BasicAttack(sf::Vector2f(1600.0f, 900.0f)); //YellowPusa Have Animations


        sf::Texture Background_BasicAttack;
        Background_BasicAttack.loadFromFile("SpriteSheets/Basic Attack Animation/BackGround/55-70.png");
        MainBackground_BasicAttack.setTexture(&Background_BasicAttack);

        sf::Texture Furina_BasicAttack;
        Furina_BasicAttack.loadFromFile("SpriteSheets/Basic Attack Animation/Character Animation/55-70.png");
        Furina_Shape_BasicAttack.setTexture(&Furina_BasicAttack);

        sf::Texture BlackPusa_BasicAttack;
        BlackPusa_BasicAttack.loadFromFile("SpriteSheets/Basic Attack Animation/Enemy Animation/55-70 Black Pusa.png");
        EnemyCharacter1_BasicAttack.setTexture(&BlackPusa_BasicAttack);

        sf::Texture YellowPusa_BasicAttack;
        YellowPusa_BasicAttack.loadFromFile("SpriteSheets/Basic Attack Animation/Enemy Animation/55-70 Yellow Pusa.png");
        EnemyCharacter2_BasicAttack.setTexture(&YellowPusa_BasicAttack);


        Animation BackGroundAnimation_BasicAttack(&Background_BasicAttack, sf::Vector2u(16, 1), 0.0833f);
        Animation FurinaAnimation_BasicAttack(&Furina_BasicAttack, sf::Vector2u(16, 1), 0.0833f);
        Animation BlackPusaAnimation_BasicAttack(&BlackPusa_BasicAttack, sf::Vector2u(16, 1), 0.0833f);
        Animation YellowPusaAnimation_BasicAttack(&YellowPusa_BasicAttack, sf::Vector2u(16, 1), 0.0833f);
        cout << " Done " << endl;





        cout << "Loading Textures 2: ";
        //SKILL ANIMATIONS
        sf::RectangleShape MainBackground_Skill(sf::Vector2f(1600.0f, 900.0f));//Have Animations
        sf::RectangleShape Furina_Shape_Skill(sf::Vector2f(1600.0f, 900.0f));
        sf::RectangleShape EnemyCharacter1_Skill(sf::Vector2f(1600.0f, 900.0f)); //BlackPusa Have Animations
        sf::RectangleShape EnemyCharacter2_Skill(sf::Vector2f(1600.0f, 900.0f)); //YellowPusa Have Animations


        sf::Texture Background_Skill;
        Background_Skill.loadFromFile("SpriteSheets/Skill Animation/BackGround/55-70.png");
        MainBackground_Skill.setTexture(&Background_Skill);

        sf::Texture Furina_Skill;
        Furina_Skill.loadFromFile("SpriteSheets/Skill Animation/Character Animation 55-70/55-70.png");
        Furina_Shape_Skill.setTexture(&Furina_Skill);

        sf::Texture BlackPusa_Skill;
        BlackPusa_Skill.loadFromFile("SpriteSheets/Skill Animation/Enemy Animation/55-70 Black.png");
        EnemyCharacter1_Skill.setTexture(&BlackPusa_Skill);

        sf::Texture YellowPusa_Skill;
        YellowPusa_Skill.loadFromFile("SpriteSheets/Skill Animation/Enemy Animation/55-70 Yellow.png");
        EnemyCharacter2_Skill.setTexture(&YellowPusa_Skill);


        Animation BackGroundAnimation_Skill(&Background_Skill, sf::Vector2u(16, 1), 0.0833f);
        Animation FurinaAnimation_Skill(&Furina_Skill, sf::Vector2u(16, 1), 0.0833f);
        Animation BlackPusaAnimation_Skill(&BlackPusa_Skill, sf::Vector2u(16, 1), 0.0833f);
        Animation YellowPusaAnimation_Skill(&YellowPusa_Skill, sf::Vector2u(16, 1), 0.0833f);
        cout << " Done " << endl;





        cout << "Loading Textures 3: ";
        //BEFORE ULTIMATE ANIMATIONS 
        sf::RectangleShape MainBackground_Before(sf::Vector2f(1600.0f, 900.0f));
        sf::RectangleShape Furina_Shape_Before(sf::Vector2f(1600.0f, 900.0f));
        sf::RectangleShape EnemyCharacter1_Before(sf::Vector2f(1600.0f, 900.0f)); //BlackPusa Have Different Angle
        sf::RectangleShape EnemyCharacter2_Before(sf::Vector2f(1600.0f, 900.0f)); //YellowPusa


        sf::Texture BackGround_Before;
        BackGround_Before.loadFromFile("SpriteSheets/Other File/3 Ultimate Animations (Unfinished Need 1)/Before (Choosing Enemy)/Background 20-50/20-50.png");
        MainBackground_Before.setTexture(&BackGround_Before);

        sf::Texture Furina_Before;
        Furina_Before.loadFromFile("SpriteSheets/Other File/3 Ultimate Animations (Unfinished Need 1)/Before (Choosing Enemy)/Character Animation/20-50.png");
        Furina_Shape_Before.setTexture(&Furina_Before);

        sf::Texture BlackPusa_Before;
        BlackPusa_Before.loadFromFile("SpriteSheets/Other File/3 Ultimate Animations (Unfinished Need 1)/Before (Choosing Enemy)/Enemy Animation 20-50/Black Pusa Still.png");
        EnemyCharacter1_Before.setTexture(&BlackPusa_Before);

        sf::Texture YellowPusa_Before;
        YellowPusa_Before.loadFromFile("SpriteSheets/Other File/3 Ultimate Animations (Unfinished Need 1)/Before (Choosing Enemy)/Enemy Animation 20-50/Yellow Pusa Still.png");
        EnemyCharacter2_Before.setTexture(&YellowPusa_Before);


        Animation FurinaAnimation(&Furina_Before, sf::Vector2u(6, 6), 0.0833f);
        cout << " Done " << endl;





        cout << "Loading Textures 4: ";
        //AFTER ULTIMATE ANIMATIONS 
        sf::RectangleShape MainBackground_After(sf::Vector2f(1600.0f, 900.0f));
        sf::RectangleShape EnemyCharacter1_After(sf::Vector2f(1600.0f, 900.0f)); //BlackPusa
        sf::RectangleShape EnemyCharacter2_After(sf::Vector2f(1600.0f, 900.0f)); //YellowPusa
        sf::RectangleShape Effect_After(sf::Vector2f(1600.0f, 900.0f));


        sf::Texture Animation2d_After;
        Animation2d_After.loadFromFile("SpriteSheets/Other File/4_1 Ultimate Animations (Unfinished Need 1)/After (Actual Ultimate Animation) 42-183/Start Base/Frame 42-183.png");
        MainBackground_After.setTexture(&Animation2d_After);

        sf::Texture BlackPusa_After;
        BlackPusa_After.loadFromFile("SpriteSheets/Other File/4_2 Ultimate Animations (Unfinished Need 1)/After (Actual Ultimate Animation) 42-183/Black Cat 153-183.png");
        EnemyCharacter1_After.setTexture(&BlackPusa_After);

        sf::Texture YellowPusa_After;
        YellowPusa_After.loadFromFile("SpriteSheets/Other File/4_2 Ultimate Animations (Unfinished Need 1)/After (Actual Ultimate Animation) 42-183/Yellow Cat 153-183.png");
        EnemyCharacter2_After.setTexture(&YellowPusa_After);

        sf::Texture Explosion_After;
        Explosion_After.loadFromFile("SpriteSheets/Other File/4_1 Ultimate Animations (Unfinished Need 1)/After (Actual Ultimate Animation) 42-183/Explosion/153-183.png");
        Effect_After.setTexture(&Explosion_After);


        Animation MainUltimateAnimation(&Animation2d_After, sf::Vector2u(12, 12), 0.0833f);
        Animation Explosion(&Explosion_After, sf::Vector2u(6, 6), 0.0833f);
        cout << " Done " << endl;





        cout << "Loading Textures 5: ";
        //FIRST IDLE LOOP ANIMATIONS 
        sf::RectangleShape MainBackground_FirstIdle(sf::Vector2f(1600.0f, 900.0f));//Have Animations
        sf::RectangleShape Furina_Shape_FirstIdle(sf::Vector2f(1600.0f, 900.0f));
        sf::RectangleShape EnemyCharacters_FirstIdle(sf::Vector2f(1600.0f, 900.0f)); //Black and Yellow Pusa Have Animations

        sf::Texture Background_FirstIdle;
        Background_FirstIdle.loadFromFile("SpriteSheets/First Loop Animation/BackGround/3-35.png");
        MainBackground_FirstIdle.setTexture(&Background_FirstIdle);

        sf::Texture Furina_FirstIdle;
        Furina_FirstIdle.loadFromFile("SpriteSheets/First Loop Animation/Character Animation/3-35.png");
        Furina_Shape_FirstIdle.setTexture(&Furina_FirstIdle);

        sf::Texture EnemyCharactersTexture_FirstIdle;
        EnemyCharactersTexture_FirstIdle.loadFromFile("SpriteSheets/First Loop Animation/Enemy Animation/3-35.png");
        EnemyCharacters_FirstIdle.setTexture(&EnemyCharactersTexture_FirstIdle);

        Animation BackGroundAnimation_FirstIdle(&Background_FirstIdle, sf::Vector2u(6, 6), 0.0833f);
        Animation FurinaAnimation_FirstIdle(&Furina_FirstIdle, sf::Vector2u(6, 6), 0.0833f);
        Animation EnemyCharactersAnimation_FirstIdle(&EnemyCharactersTexture_FirstIdle, sf::Vector2u(6, 6), 0.0833f);
        cout << " Done " << endl;





        cout << "Loading Textures 6: ";
        //BREATHING LOOP ANIMATIONS 
        sf::RectangleShape MainBackground_Breathing(sf::Vector2f(1600.0f, 900.0f));//Have Animations
        sf::RectangleShape Furina_Shape_Breathing(sf::Vector2f(1600.0f, 900.0f));
        sf::RectangleShape EnemyCharacter1_Breathing(sf::Vector2f(1600.0f, 900.0f)); //BlackPusa Have Animations
        sf::RectangleShape EnemyCharacter2_Breathing(sf::Vector2f(1600.0f, 900.0f)); //YellowPusa Have Animations

        sf::Texture Background_Breathing;
        Background_Breathing.loadFromFile("SpriteSheets/Breathing Loop Animation/BackGround/20-50 still.png");
        MainBackground_Breathing.setTexture(&Background_Breathing);

        sf::Texture Furina_Breathing;
        Furina_Breathing.loadFromFile("SpriteSheets/Breathing Loop Animation/Character Animation/20-50.png");
        Furina_Shape_Breathing.setTexture(&Furina_Breathing);

        sf::Texture BlackPusa_Breathing;
        BlackPusa_Breathing.loadFromFile("SpriteSheets/Breathing Loop Animation/Enemy Animation/20-50 Black.png");
        EnemyCharacter1_Breathing.setTexture(&BlackPusa_Breathing);

        sf::Texture YellowPusa_Breathing;
        YellowPusa_Breathing.loadFromFile("SpriteSheets/Breathing Loop Animation/Enemy Animation/20-50 Yellow.png");
        EnemyCharacter2_Breathing.setTexture(&YellowPusa_Breathing);

        Animation FurinaAnimation_Breathing(&Furina_Breathing, sf::Vector2u(6, 6), 0.0833f);
        cout << " Done " << endl;





        cout << "Loading Textures 7: ";
        //BLACK PUSA ATTACKING ANIMATIONS 
        sf::RectangleShape MainBackground_BlackPusa(sf::Vector2f(1600.0f, 900.0f));//Have Animations
        sf::RectangleShape Furina_Shape_BlackPusa(sf::Vector2f(1600.0f, 900.0f));
        sf::RectangleShape EnemyCharacter1_BlackPusa(sf::Vector2f(1600.0f, 900.0f)); //BlackPusa Have Animations
        sf::RectangleShape EnemyCharacter2_BlackPusa(sf::Vector2f(1600.0f, 900.0f)); //YellowPusa Have Animations

        sf::Texture Background_BlackPusa;
        Background_BlackPusa.loadFromFile("SpriteSheets/Pusa Attacking/Black Pusa/Background/54-60.png");
        MainBackground_BlackPusa.setTexture(&Background_BlackPusa);

        sf::Texture Furina_BlackPusa;
        Furina_BlackPusa.loadFromFile("SpriteSheets/Pusa Attacking/Black Pusa/Character Animation 54-60/54-60.png");
        Furina_Shape_BlackPusa.setTexture(&Furina_BlackPusa);

        sf::Texture BlackPusa_BlackPusa;
        BlackPusa_BlackPusa.loadFromFile("SpriteSheets/Pusa Attacking/Black Pusa/Enemy Animation/54-60 Black.png");
        EnemyCharacter1_BlackPusa.setTexture(&BlackPusa_BlackPusa);

        sf::Texture YellowPusa_BlackPusa;
        YellowPusa_BlackPusa.loadFromFile("SpriteSheets/Pusa Attacking/Black Pusa/Enemy Animation/54-60 Yellow.png");
        EnemyCharacter2_BlackPusa.setTexture(&YellowPusa_BlackPusa);

        Animation BackGroundAnimation_BlackPusa(&Background_BlackPusa, sf::Vector2u(7, 1), 0.0833f);
        Animation FurinaAnimation_BlackPusa(&Furina_BlackPusa, sf::Vector2u(7, 1), 0.0833f);
        Animation BlackPusaAnimation_BlackPusa(&BlackPusa_BlackPusa, sf::Vector2u(7, 1), 0.0833f);
        Animation YellowPusaAnimation_BlackPusa(&YellowPusa_BlackPusa, sf::Vector2u(7, 1), 0.0833f);
        cout << " Done " << endl;





        cout << "Loading Textures 8: ";
        //YELLOW PUSA ATTACKING ANIMATIONS
        sf::RectangleShape MainBackground_YellowPusa(sf::Vector2f(1600.0f, 900.0f));//Have Animations
        sf::RectangleShape Furina_Shape_YellowPusa(sf::Vector2f(1600.0f, 900.0f));
        sf::RectangleShape EnemyCharacter1_YellowPusa(sf::Vector2f(1600.0f, 900.0f)); //BlackPusa Have Animations
        sf::RectangleShape EnemyCharacter2_YellowPusa(sf::Vector2f(1600.0f, 900.0f)); //YellowPusa Have Animations

        sf::Texture Background_YellowPusa;
        Background_YellowPusa.loadFromFile("SpriteSheets/Pusa Attacking/Yellow Pusa/Background/40-47.png");
        MainBackground_YellowPusa.setTexture(&Background_YellowPusa);

        sf::Texture Furina_YellowPusa;
        Furina_YellowPusa.loadFromFile("SpriteSheets/Pusa Attacking/Yellow Pusa/Character Animation/40-47.png");
        Furina_Shape_YellowPusa.setTexture(&Furina_YellowPusa);

        sf::Texture BlackPusa_YellowPusa;
        BlackPusa_YellowPusa.loadFromFile("SpriteSheets/Pusa Attacking/Yellow Pusa/Enemy Animation/40-47 Black.png");
        EnemyCharacter1_YellowPusa.setTexture(&BlackPusa_YellowPusa);

        sf::Texture YellowPusa_YellowPusa;
        YellowPusa_YellowPusa.loadFromFile("SpriteSheets/Pusa Attacking/Yellow Pusa/Enemy Animation/40-47 Yellow.png");
        EnemyCharacter2_YellowPusa.setTexture(&YellowPusa_YellowPusa);

        Animation BackGroundAnimation_YellowPusa(&Background_YellowPusa, sf::Vector2u(8, 1), 0.0833f);
        Animation FurinaAnimation_YellowPusa(&Furina_YellowPusa, sf::Vector2u(8, 1), 0.0833f);
        Animation BlackPusaAnimation_YellowPusa(&BlackPusa_YellowPusa, sf::Vector2u(8, 1), 0.0833f);
        Animation YellowPusaAnimation_YellowPusa(&YellowPusa_YellowPusa, sf::Vector2u(8, 1), 0.0833f);
        cout << " Done " << endl; 
    
        cout << "Done Loading All Textures!!!" << endl;
        system("cls");
    #pragma endregion


    #pragma region CHARACTER DEFAULT VALUES
    /*
        name, speed, health, attack, player, EnergyRegenRate, EnergyInside (Ultimate cooldown), Crit Rate, Crit Dmg,IsDead ,  IsEnemy1 2 = Main 0 = False 1 = True
        {"Furina", 100, 1630, 84, true, 0.194, 0, 50, 0.50, false,2, 0},//hp==163
        {"Nigga Cat", 83, 1000, 12, false,0,0,50,0.25, false,1, 1}, //hp == 97
        {"Chonker Cat", 83, 1000, 12, false,0,0,50,0.25, false,0, 2} //hp == 97
    */
    #pragma endregion
    
    #pragma region // Initialize game data   
        vector<Character> characters;

        //Get character from config.json (limit 3)
            int count = 0;
            for (auto& j : config["characters"])
            {
                if (count >= 3)
                {
                    std::cout << "Warning: extra characters ignored in config.json\n";
                    break;
                }

                Character c;

                c.Name = j["name"];
                c.Speed = j["speed"];
                c.Health = j["health"];
                c.Attack = j["attack"];
                c.Player = j["player"];

                c.ERRate = j["errate"];
                c.EnergyInside = j["energyInside"];
                c.CritRate = j["critRate"];
                c.CritDamage = j["critDamage"];

                c.IsDead = j["isDead"];
                c.IsEnemy1 = j["isEnemy1"];
                c.WhatCharacter = j["whatCharacter"];

                characters.push_back(c);

                count++;
            }

        //config.json
            int initialActionGauge = config["initialActionGauge"]; // default: 10000
    
        //Calculate action value for the characters (only 3)
        for (auto& character : characters)
        {
            character.ActionValue = CalculateActionValue(initialActionGauge, character.Speed);
        }
        ActionValueSorter(characters);

        //config.json
            int SkillPoint = config["skillPoints"]; // default: 3

        int PlayerChoice = 0;
        int CharacterDamage = 0;
        int ChoosenCharacter = 0;
        bool IsCritAttack = false;
        bool TurnOrderDisplayed = false;
        bool OtherCodeDisplayed = false;
        bool HpDisplayed = false;
        bool AttackRight = false;
        bool CharacterUI = false;
        bool YouWinorLoseDisplay = false;

        //Frame Clock
        sf::Clock clock;
        float DeltaTime = 0.0f;
        int AnimationNumber = 1;
        bool StartingAnimationBase = false;
        int CurrentTurn = 0;
        bool FurinaIsDead = false;
        bool BlackPusaIsDead = false;
        bool YellowPusaIsDead = false;

        bool FurinaIsDeadLimiter = false;
        bool BlackPusaIsDeadLimiter = false;
        bool YellowPusaIsDeadLimiter = false;
    #pragma endregion


    //0 = Blank
    //1 = First Idle Loop
    //2 = Breathing Loop
    //3 = Basic Attack
    //4 = Skill
    //5 = Black Pusa = Enemy 1
    //6 = Yellow Pusa = Enemy 2
    //7 = Before Ultimate 
    //8 = After Ultimate
    while (Window.isOpen())
    {
        //Frame Clock
        DeltaTime = clock.restart().asSeconds();
        //cout << "DeltaTime: " << DeltaTime << endl;

        if (!TurnOrderDisplayed)
        {
            cout << "Turn Order: " << endl;
            for (auto& character : characters)
            {
                cout << character.Name << " - Current Action Gauge: " << character.ActionValue << "\n";
            }
            cout << "\n\n\n\n";
            TurnOrderDisplayed = true;
        }

        if (!HpDisplayed)
        {
            for (auto& character : characters)
            {
                cout << character.Name << " Remaining HP: " << character.Health << "\n";
            }
            HpDisplayed = true;
        }

        if (!CharacterUI)
        {
            for (auto& character : characters)
            {
                if (character.Name == "Furina")
                {
                    DisplayUI(SkillPoint, character.EnergyInside);
                }
            }
            CharacterUI = true;
        }

        if (!BlackPusaIsDeadLimiter)
        {
            for (auto& character : characters)
            {
                if (character.WhatCharacter == 1)
                {
                    if (character.IsDead == true)
                    {
                        BlackPusaIsDead = true;
                    }
                }
            }
            BlackPusaIsDeadLimiter = true;
        }

        if (!YellowPusaIsDeadLimiter)
        {
            for (auto& character : characters)
            {
                if (character.WhatCharacter == 2)
                {
                    if (character.IsDead == true)
                    {
                        YellowPusaIsDead = true;
                    }
                }
            }
            YellowPusaIsDeadLimiter = true;
        }


        if (BlackPusaIsDead == true && YellowPusaIsDead == true)
        {
            if (YouWinorLoseDisplay == false)
            {
                AnimationNumber = 0;
                cout << "YOU WIN!!" << endl;
                YouWinorLoseDisplay = true;
            }
        }
            

        if (FurinaIsDead == true)
        {
            if (YouWinorLoseDisplay == false)
            {
                AnimationNumber = 0;
                cout << "YOU LOSE!!" << endl;
                YouWinorLoseDisplay = true;
            }
        }
        



        sf::Event event;
        while (Window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                Window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            //std::cout << "Goods Pa!!" << std::endl;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
        {
            //cout << "R" << endl;
            AnimationNumber = 1;
        }



        if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
        {
            //cout << "M becomes true" << endl;
            OtherCodeDisplayed = false;
        }
        if (StartingAnimationBase == false)
        {
            if (characters[CurrentTurn].Player == 1)
            {
                AnimationNumber = 1;
                StartingAnimationBase = true;
            }
            else
            {
                if (characters[CurrentTurn].IsEnemy1 == 1)
                {
                    AnimationNumber = 5;
                    StartingAnimationBase = true;
                }
                else
                {
                    AnimationNumber = 6;
                    StartingAnimationBase = true;
                }
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
        {
            //cout << "Current Animation Number: " << AnimationNumber << endl;
        }

        if (AnimationNumber == 0)//Blank Test
        {
            BackGroundAnimation_FirstIdle.AnimationReset(BackGroundAnimation_FirstIdle.CurrentImage);
            FurinaAnimation_FirstIdle.AnimationReset(FurinaAnimation_FirstIdle.CurrentImage);
            EnemyCharactersAnimation_FirstIdle.AnimationReset(EnemyCharactersAnimation_FirstIdle.CurrentImage);
            Window.clear();
            Window.display();
        }

            //0 = Blank
            //1 = First Idle Loop
            //2 = Breathing Loop
            //3 = Basic Attack
            //4 = Skill
            //5 = Black Pusa = Enemy 1
            //6 = Yellow Pusa = Enemy 2
        if (AnimationNumber == 1) //First Loop to Breathing
        {
            BackGroundAnimation_FirstIdle.UpdateGrid(0, DeltaTime, sf::Vector2u(3, 5));
            MainBackground_FirstIdle.setTextureRect(BackGroundAnimation_FirstIdle.UvRect);

            FurinaAnimation_FirstIdle.UpdateGrid(0, DeltaTime, sf::Vector2u(3, 5));
            Furina_Shape_FirstIdle.setTextureRect(FurinaAnimation_FirstIdle.UvRect);

            EnemyCharactersAnimation_FirstIdle.UpdateGrid(0, DeltaTime, sf::Vector2u(3, 5));
            EnemyCharacters_FirstIdle.setTextureRect(EnemyCharactersAnimation_FirstIdle.UvRect);

            Window.clear();
            Window.draw(MainBackground_FirstIdle);
            Window.draw(Furina_Shape_FirstIdle);
            Window.draw(EnemyCharacters_FirstIdle);
            Window.display();

            if (FurinaAnimation_FirstIdle.CurrentImage.y == 5)
            {
                if (FurinaAnimation_FirstIdle.CurrentImage.x >= 2)
                {
                    BackGroundAnimation_FirstIdle.AnimationReset(BackGroundAnimation_FirstIdle.CurrentImage);
                    FurinaAnimation_FirstIdle.AnimationReset(FurinaAnimation_FirstIdle.CurrentImage);
                    EnemyCharactersAnimation_FirstIdle.AnimationReset(EnemyCharactersAnimation_FirstIdle.CurrentImage);
                    AnimationNumber = 2;
                }
            }
        }

        if (AnimationNumber == 2)//Breathing Loop
        {
            FurinaAnimation_Breathing.UpdateGrid(0, DeltaTime, sf::Vector2u(1, 5));
            Furina_Shape_Breathing.setTextureRect(FurinaAnimation_Breathing.UvRect);
            Window.clear();
            Window.draw(MainBackground_Breathing);

            for (auto& character : characters)
            {
                if (character.Name == "Maxwell Cat")
                {
                    if (character.IsDead == false)
                    {
                        Window.draw(EnemyCharacter1_Breathing);
                    }
                }
            }

            for (auto& character : characters)
            {
                if (character.Name == "Chonker Cat")
                {
                    if (character.IsDead == false)
                    {
                        Window.draw(EnemyCharacter2_Breathing);
                    }
                }
            }

            Window.draw(Furina_Shape_Breathing);
            Window.display();



            if (AnimationNumber == 2)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                {
                    cout << "Space" << endl;
                    if (characters[CurrentTurn].EnergyInside >= 120)
                    {
                        AnimationNumber = 7;
                    }
                    else
                    {
                        cout << "Not Enough Energy!!!!" << endl;
                    }
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                {
                    AttackRight = false;
                    cout << "Left ";
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                {
                    AttackRight = true;
                    cout << "Right ";
                }
                
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
                {
                    std::cout << "Q" << std::endl;
                    if (AttackRight == true)
                    {
                        //problems is its tru
                        //if false do animationumber = 3
                        if (BlackPusaIsDead == false)
                        {
                            AnimationNumber = 3;
                        }
                        else
                        {
                            cout << "Cant kill a dead being!!!" << endl;
                            BlackPusaIsDeadLimiter = true;
                        }
                    }
                    else
                    {
                        if (YellowPusaIsDead == false)
                        {
                            AnimationNumber = 3;
                        }
                        else
                        {
                            cout << "Cant kill a dead being!!!" << endl;
                            YellowPusaIsDeadLimiter = true;
                        }
                    }


                    FurinaAnimation_Breathing.AnimationReset(FurinaAnimation_Breathing.CurrentImage);
                    //CurrentTurn++;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
                {
                    if (SkillPoint > 0)
                    {
                        std::cout << "E" << std::endl;

                        if (AttackRight == true)
                        {
                            //problems is its tru
                            //if false do animationumber = 3
                            if (BlackPusaIsDead == false)
                            {
                                AnimationNumber = 4;
                            }
                            else
                            {
                                cout << "Cant kill a dead being!!!" << endl;
                                BlackPusaIsDeadLimiter = true;
                            }
                        }
                        else
                        {
                            if (YellowPusaIsDead == false)
                            {
                                AnimationNumber = 4;
                            }
                            else
                            {
                                cout << "Cant kill a dead being!!!" << endl;
                                YellowPusaIsDeadLimiter = true;
                            }
                        }
                    }
                    else
                    {
                        cout << "Not Enough Skill Point!!!" << endl;
                    }
                    
                    
                    
                }




            }

        }

        if (AnimationNumber == 3)//Basic Attack Animation
        {
            BackGroundAnimation_BasicAttack.UpdateHorizontal(0, DeltaTime);
            MainBackground_BasicAttack.setTextureRect(BackGroundAnimation_BasicAttack.UvRect);

            YellowPusaAnimation_BasicAttack.UpdateHorizontal(0, DeltaTime);
            EnemyCharacter2_BasicAttack.setTextureRect(YellowPusaAnimation_BasicAttack.UvRect);

            BlackPusaAnimation_BasicAttack.UpdateHorizontal(0, DeltaTime);
            EnemyCharacter1_BasicAttack.setTextureRect(BlackPusaAnimation_BasicAttack.UvRect);

            if (FurinaAnimation_BasicAttack.CurrentImage.x <= 4)
            {
                if (AttackRight == true)
                {
                    EnemyCharacter2_BasicAttack.setPosition(-250, 0);
                    EnemyCharacter1_BasicAttack.setPosition(-250, 0);
                }
            }

            FurinaAnimation_BasicAttack.UpdateHorizontal(0, DeltaTime);
            Furina_Shape_BasicAttack.setTextureRect(FurinaAnimation_BasicAttack.UvRect);

            Window.clear();
            Window.draw(MainBackground_BasicAttack);

            for (auto& character : characters)
            {
                if (character.Name == "Maxwell Cat")
                {
                    if (character.IsDead == false)
                    {
                        Window.draw(EnemyCharacter1_BasicAttack);
                    }
                }
            }

            for (auto& character : characters)
            {
                if (character.Name == "Chonker Cat")
                {
                    if (character.IsDead == false)
                    {
                        Window.draw(EnemyCharacter2_BasicAttack);
                    }
                }
            }

            Window.draw(Furina_Shape_BasicAttack);
            Window.display();
            //cout << "BELOWEST EXECUTED" << endl;
            if (FurinaAnimation_BasicAttack.CurrentImage.x == 15)
            {
                BackGroundAnimation_BasicAttack.AnimationReset(BackGroundAnimation_BasicAttack.CurrentImage);
                YellowPusaAnimation_BasicAttack.AnimationReset(YellowPusaAnimation_BasicAttack.CurrentImage);
                BlackPusaAnimation_BasicAttack.AnimationReset(BlackPusaAnimation_BasicAttack.CurrentImage);
                FurinaAnimation_BasicAttack.AnimationReset(FurinaAnimation_BasicAttack.CurrentImage);

                EnemyCharacter2_BasicAttack.setPosition(0, 0);
                EnemyCharacter1_BasicAttack.setPosition(0, 0);

                CharacterDamage = CalculateBaseDamage(0.50, characters[CurrentTurn].Attack);
                IsCritAttack = CritIdentifier(characters[CurrentTurn].CritRate);
                if (IsCritAttack)
                {
                    CharacterDamage *= 1 + characters[CurrentTurn].CritDamage;
                }

                if (AttackRight == true)
                {
                    for (auto& character : characters)
                    {
                        if (character.Name == "Maxwell Cat")
                        {
                            character.Health = ApplyDamageAndLimiter(character.Health, CharacterDamage, character.IsDead);
                            if (character.IsDead == true)
                            {
                                BlackPusaIsDead = true;
                            }
                            if (character.Health <= 0)
                            {
                                characters[CurrentTurn].EnergyInside = AddAndDisplayEnergyPointAndLimiter(characters[CurrentTurn].EnergyInside, characters[CurrentTurn].ERRate, 10);
                            }
                            SkillPoint = AddAndDisplaySkillPointAndLimiter(SkillPoint);
                            characters[CurrentTurn].EnergyInside = AddAndDisplayEnergyPointAndLimiter(characters[CurrentTurn].EnergyInside, characters[CurrentTurn].ERRate, 20);

                            //HpDisplayed = false;
                            //system("cls"); COMMENT
                            if (HpDisplayed == false)
                            {
                                for (auto& charactering : characters)
                                {
                                    cout << charactering.Name << " Remaining HP: " << charactering.Health << "\n";
                                }
                                HpDisplayed = true;
                            }

                        }
                    }
                }
                else
                {
                    for (auto& character : characters)
                    {
                        if (character.Name == "Chonker Cat")
                        {
                            character.Health = ApplyDamageAndLimiter(character.Health, CharacterDamage, character.IsDead);
                            if (character.IsDead == true)
                            {
                                YellowPusaIsDead = true;
                            }
                            if (character.Health <= 0)
                            {
                                characters[CurrentTurn].EnergyInside = AddAndDisplayEnergyPointAndLimiter(character.EnergyInside, character.ERRate, 10);
                            }
                            SkillPoint = AddAndDisplaySkillPointAndLimiter(SkillPoint);
                            characters[CurrentTurn].EnergyInside = AddAndDisplayEnergyPointAndLimiter(characters[CurrentTurn].EnergyInside, characters[CurrentTurn].ERRate, 20);

                            //HpDisplayed = false;
                            //system("cls"); COMMENT
                            if (HpDisplayed == false)
                            {
                                for (auto& charactering : characters)
                                {
                                    cout << charactering.Name << " Remaining HP: " << charactering.Health << "\n";
                                }
                                HpDisplayed = true;
                            }
                        }
                    }
                }

                CurrentTurn++;
                if (CurrentTurn == 3)
                {
                    CurrentTurn = 0;
                }

                if (characters[CurrentTurn].Player == 1)
                {
                    AnimationNumber = 2;
                }
                else
                {
                    if (characters[CurrentTurn].IsEnemy1 == 1)
                    {
                        AnimationNumber = 5;
                    }
                    else
                    {
                        AnimationNumber = 6;
                    }
                }
            }
        }

        if (AnimationNumber == 4) // Skill Animation
        {
            BackGroundAnimation_Skill.UpdateHorizontal(0, DeltaTime);
            MainBackground_Skill.setTextureRect(BackGroundAnimation_Skill.UvRect);

            BlackPusaAnimation_Skill.UpdateHorizontal(0, DeltaTime);
            EnemyCharacter1_Skill.setTextureRect(BlackPusaAnimation_Skill.UvRect);

            YellowPusaAnimation_Skill.UpdateHorizontal(0, DeltaTime);
            EnemyCharacter2_Skill.setTextureRect(YellowPusaAnimation_Skill.UvRect);

            if (FurinaAnimation_BasicAttack.CurrentImage.x <= 9)
            {
                if (AttackRight == true)
                {
                    EnemyCharacter1_Skill.setPosition(-250, 0);
                    EnemyCharacter2_Skill.setPosition(-250, 0);
                }
            }

            FurinaAnimation_Skill.UpdateHorizontal(0, DeltaTime);
            Furina_Shape_Skill.setTextureRect(FurinaAnimation_Skill.UvRect);

            Window.clear();
            Window.draw(MainBackground_Skill);

            for (auto& character : characters)
            {
                if (character.Name == "Maxwell Cat")
                {
                    if (character.IsDead == false)
                    {
                        Window.draw(EnemyCharacter1_Skill);
                    }
                }
            }

            for (auto& character : characters)
            {
                if (character.Name == "Chonker Cat")
                {
                    if (character.IsDead == false)
                    {
                        Window.draw(EnemyCharacter2_Skill);
                    }
                }
            }



            Window.draw(Furina_Shape_Skill);
            Window.display();

            if (FurinaAnimation_Skill.CurrentImage.x == 15)
            {
                BackGroundAnimation_Skill.AnimationReset(BackGroundAnimation_Skill.CurrentImage);
                BlackPusaAnimation_Skill.AnimationReset(BlackPusaAnimation_Skill.CurrentImage);
                YellowPusaAnimation_Skill.AnimationReset(YellowPusaAnimation_Skill.CurrentImage);
                FurinaAnimation_Skill.AnimationReset(FurinaAnimation_Skill.CurrentImage);

                EnemyCharacter1_Skill.setPosition(0, 0);
                EnemyCharacter2_Skill.setPosition(0, 0);

                CharacterDamage = CalculateBaseDamage(0.625, characters[CurrentTurn].Attack);
                IsCritAttack = CritIdentifier(characters[CurrentTurn].CritRate);
                if (IsCritAttack)
                {
                    CharacterDamage *= 1 + characters[CurrentTurn].CritDamage;
                }

                if (AttackRight == true)
                {
                    for (auto& character : characters)
                    {
                        if (character.Name == "Maxwell Cat")
                        {
                            character.Health = ApplyDamageAndLimiter(character.Health, CharacterDamage, character.IsDead);
                            if (character.IsDead == true)
                            {
                                BlackPusaIsDead = true;
                            }
                            if (character.Health <= 0)
                            {
                                characters[CurrentTurn].EnergyInside = AddAndDisplayEnergyPointAndLimiter(characters[CurrentTurn].EnergyInside, characters[CurrentTurn].ERRate, 10);
                            }
                            SkillPoint = SubtractAndDisplaySkillPointAndLimiter(SkillPoint);
                            characters[CurrentTurn].EnergyInside = AddAndDisplayEnergyPointAndLimiter(characters[CurrentTurn].EnergyInside, characters[CurrentTurn].ERRate, 20);

                            //HpDisplayed = false;
                            //system("cls"); COMMENT
                            if (HpDisplayed == false)
                            {
                                for (auto& charactering : characters)
                                {
                                    cout << charactering.Name << " Remaining HP: " << charactering.Health << "\n";
                                }
                                HpDisplayed = true;
                            }

                        }
                    }
                }
                else
                {
                    for (auto& character : characters)
                    {
                        if (character.Name == "Chonker Cat")
                        {
                            character.Health = ApplyDamageAndLimiter(character.Health, CharacterDamage, character.IsDead);
                            if (character.IsDead == true)
                            {
                                YellowPusaIsDead = true;
                            }
                            if (character.Health <= 0)
                            {
                                characters[CurrentTurn].EnergyInside = AddAndDisplayEnergyPointAndLimiter(character.EnergyInside, character.ERRate, 10);
                            }
                            SkillPoint = SubtractAndDisplaySkillPointAndLimiter(SkillPoint);
                            characters[CurrentTurn].EnergyInside = AddAndDisplayEnergyPointAndLimiter(characters[CurrentTurn].EnergyInside, characters[CurrentTurn].ERRate, 20);

                            //HpDisplayed = false;
                            //system("cls"); COMMENT
                            if (HpDisplayed == false)
                            {
                                for (auto& charactering : characters)
                                {
                                    cout << charactering.Name << " Remaining HP: " << charactering.Health << "\n";
                                }
                                HpDisplayed = true;
                            }
                        }
                    }
                }
                CurrentTurn++;
                if (CurrentTurn == 3)
                {
                    CurrentTurn = 0;
                }

                if (characters[CurrentTurn].Player == 1)
                {
                    AnimationNumber = 2;
                }
                else
                {
                    if (characters[CurrentTurn].IsEnemy1 == 1)
                    {
                        AnimationNumber = 5;
                    }
                    else
                    {
                        AnimationNumber = 6;
                    }
                }
            }
        }


        /*for (auto& character : characters)
        {
            if (character.WhatCharacter == 1)
            {
                if (character.IsDead == true)
                {
                    BlackPusaIsDead = true;
                }
            }
        }*/

        //if (BlackPusaIsDead == false)
        //{
        //cout << "BLACKPUSAISDEAD: " << BlackPusaIsDead << endl;
        /*if (BlackPusaIsDead == false)
        {*/
        if (AnimationNumber == 5) //Black Pusa
        {

            //EnemyCharacter1 = Black Pusa  EnemyCharacter2 Yellow Pusa
            //
            BackGroundAnimation_BlackPusa.UpdateHorizontal(0, DeltaTime);
            MainBackground_BlackPusa.setTextureRect(BackGroundAnimation_BlackPusa.UvRect);

            BlackPusaAnimation_BlackPusa.UpdateHorizontal(0, DeltaTime);
            EnemyCharacter1_BlackPusa.setTextureRect(BlackPusaAnimation_BlackPusa.UvRect);

            YellowPusaAnimation_BlackPusa.UpdateHorizontal(0, DeltaTime);
            EnemyCharacter2_BlackPusa.setTextureRect(YellowPusaAnimation_BlackPusa.UvRect);

            FurinaAnimation_BlackPusa.UpdateHorizontal(0, DeltaTime);
            Furina_Shape_BlackPusa.setTextureRect(FurinaAnimation_BlackPusa.UvRect);

            Window.clear();
            Window.draw(MainBackground_BlackPusa);

            for (auto& character : characters)
            {
                if (character.Name == "Chonker Cat")
                {
                    if (character.IsDead == false)
                    {
                        Window.draw(EnemyCharacter2_BlackPusa);
                    }
                }
            }

            Window.draw(EnemyCharacter1_BlackPusa);
            Window.draw(Furina_Shape_BlackPusa);
            Window.display();

            if (FurinaAnimation_BlackPusa.CurrentImage.x == 6)
            {
                BackGroundAnimation_BlackPusa.AnimationReset(BackGroundAnimation_BlackPusa.CurrentImage);
                BlackPusaAnimation_BlackPusa.AnimationReset(BlackPusaAnimation_BlackPusa.CurrentImage);
                YellowPusaAnimation_BlackPusa.AnimationReset(YellowPusaAnimation_BlackPusa.CurrentImage);
                FurinaAnimation_BlackPusa.AnimationReset(FurinaAnimation_BlackPusa.CurrentImage);

                CharacterDamage = CalculateBaseDamage(3.0, characters[CurrentTurn].Attack);
                IsCritAttack = CritIdentifier(characters[CurrentTurn].CritRate);
                if (IsCritAttack)
                {
                    CharacterDamage *= 1 + characters[CurrentTurn].CritDamage;
                }
                for (auto& character : characters)
                {
                    if (character.Name == "Furina")
                    {
                        character.Health = ApplyDamageAndLimiter(character.Health, CharacterDamage, character.IsDead);
                        if (character.Health <= 0)
                        {
                            FurinaIsDead = true;
                        }
                        character.EnergyInside = AddAndDisplayEnergyPointAndLimiter(character.EnergyInside, character.ERRate, 2);
                    }
                }

                if (FurinaIsDead == false)
                {
                    HpDisplayed = false;
                    TurnOrderDisplayed = false;
                    CharacterUI = false;
                }
                
                CurrentTurn++;
                if (CurrentTurn == 3)
                {
                    CurrentTurn = 0;
                }

                if (characters[CurrentTurn].Player == 1)
                {
                    AnimationNumber = 2;
                }
                else
                {
                    if (characters[CurrentTurn].IsEnemy1 == 1)
                    {
                        if (characters[CurrentTurn].IsDead == true)
                        {
                            AnimationNumber = 2;
                        }
                        else
                        {
                            AnimationNumber = 5;
                        }

                    }
                    else
                    {
                        if (characters[CurrentTurn].IsDead == true)
                        {
                            AnimationNumber = 2;
                        }
                        else
                        {
                            AnimationNumber = 6;
                        }
                    }
                }
            }
        }
        //}

        /*else
        {
            cout << "CODE TRIGGERED" << endl;
            CurrentTurn++;
            if (CurrentTurn == 3)
            {
                CurrentTurn = 0;
            }

            if (characters[CurrentTurn].Player == 1)
            {
                AnimationNumber = 2;
            }
            else
            {
                if (characters[CurrentTurn].IsEnemy1 == 1)
                {
                    if (characters[CurrentTurn].IsDead == true)
                    {
                        AnimationNumber = 6;
                    }

                }
            }
        }*/

        //}
        /*else COMMENT
        {
            CurrentTurn++;
            if (CurrentTurn == 3)
            {
                CurrentTurn = 0;
            }

            if (characters[CurrentTurn].Player == 1)
            {
                AnimationNumber = 2;
            }
            else
            {
                if (characters[CurrentTurn].IsEnemy1 == 1)
                {
                    if (characters[CurrentTurn].IsDead == true)
                    {
                        AnimationNumber = 2;
                    }
                    else
                    {
                        AnimationNumber = 5;
                    }

                }
                else
                {
                    if (characters[CurrentTurn].IsDead == true)
                    {
                        AnimationNumber = 2;
                    }
                    else
                    {
                        AnimationNumber = 6;
                    }
                }
            }
        }*/
        //cout << "YELLOWPUSAISDEAD: " << YellowPusaIsDead << endl;
        /*if (YellowPusaIsDead == false)
        {*/
        //cout << "VERY STUCKED " << endl;
        if (AnimationNumber == 6) //Yellow Pusa
        {
            //cout << "STUCKED ABOVE " << endl;
            BackGroundAnimation_YellowPusa.UpdateHorizontal(0, DeltaTime);
            MainBackground_YellowPusa.setTextureRect(BackGroundAnimation_YellowPusa.UvRect);

            BlackPusaAnimation_YellowPusa.UpdateHorizontal(0, DeltaTime);
            EnemyCharacter1_YellowPusa.setTextureRect(BlackPusaAnimation_YellowPusa.UvRect);

            YellowPusaAnimation_YellowPusa.UpdateHorizontal(0, DeltaTime);
            EnemyCharacter2_YellowPusa.setTextureRect(YellowPusaAnimation_YellowPusa.UvRect);

            FurinaAnimation_YellowPusa.UpdateHorizontal(0, DeltaTime);
            Furina_Shape_YellowPusa.setTextureRect(FurinaAnimation_YellowPusa.UvRect);

            Window.clear();
            Window.draw(MainBackground_YellowPusa);

            for (auto& character : characters)
            {
                if (character.Name == "Maxwell Cat")
                {
                    if (character.IsDead == false)
                    {
                        Window.draw(EnemyCharacter1_YellowPusa);
                        //cout << "STUCKED" << endl;
                    }
                }
            }

            Window.draw(EnemyCharacter2_YellowPusa);
            Window.draw(Furina_Shape_YellowPusa);
            Window.display();

            if (FurinaAnimation_YellowPusa.CurrentImage.x == 7)
            {
                //cout << "CODE PASSED " << endl;
                BackGroundAnimation_YellowPusa.AnimationReset(BackGroundAnimation_YellowPusa.CurrentImage);
                BlackPusaAnimation_YellowPusa.AnimationReset(BlackPusaAnimation_YellowPusa.CurrentImage);
                YellowPusaAnimation_YellowPusa.AnimationReset(YellowPusaAnimation_YellowPusa.CurrentImage);
                FurinaAnimation_YellowPusa.AnimationReset(FurinaAnimation_YellowPusa.CurrentImage);

                CharacterDamage = CalculateBaseDamage(3.0, characters[CurrentTurn].Attack);
                IsCritAttack = CritIdentifier(characters[CurrentTurn].CritRate);
                if (IsCritAttack)
                {
                    CharacterDamage *= 1 + characters[CurrentTurn].CritDamage;
                }
                for (auto& character : characters)
                {
                    if (character.Name == "Furina")
                    {
                        character.Health = ApplyDamageAndLimiter(character.Health, CharacterDamage, character.IsDead);
                        if (character.Health <= 0)
                        {
                            FurinaIsDead = true;
                        }
                        character.EnergyInside = AddAndDisplayEnergyPointAndLimiter(character.EnergyInside, character.ERRate, 2);
                    }
                }

                if (FurinaIsDead == false)
                {
                    HpDisplayed = false;
                    TurnOrderDisplayed = false;
                    CharacterUI = false;
                }

                //system("cls"); COMMENT

                /*for (auto& character : characters)
                {
                    if (character.Name == "Furina")
                    {
                        DisplayUI(SkillPoint, character.EnergyInside);
                    }
                }*/

                CurrentTurn++;
                if (CurrentTurn == 3)
                {
                    CurrentTurn = 0;
                }

                if (characters[CurrentTurn].Player == 1)
                {
                    AnimationNumber = 2;
                }
                else
                {
                    if (characters[CurrentTurn].IsEnemy1 == 1)
                    {
                        AnimationNumber = 5;
                    }
                    else
                    {
                        AnimationNumber = 6;
                    }
                }
            }
        }



        if (AnimationNumber == 7) //Before Ultimate
        {
            FurinaAnimation.UpdateGrid(0, DeltaTime, sf::Vector2u(1, 5));
            Furina_Shape_Before.setTextureRect(FurinaAnimation.UvRect);

            Window.clear();
            Window.draw(MainBackground_Before);
            Window.draw(Furina_Shape_Before);

            for (auto& character : characters)
            {
                if (character.Name == "Maxwell Cat")
                {
                    if (character.IsDead == false)
                    {
                        Window.draw(EnemyCharacter1_Before);
                    }
                }
            }

            for (auto& character : characters)
            {
                if (character.Name == "Chonker Cat")
                {
                    if (character.IsDead == false)
                    {
                        Window.draw(EnemyCharacter2_Before);
                    }
                }
            }

            Window.display();

            if (AnimationNumber == 7)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                {
                    AttackRight = false;
                    cout << "Left ";
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                {
                    AttackRight = true;
                    cout << "Right ";
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                {
                    std::cout << "Space" << std::endl;
                    if (AttackRight == true)
                    {
                        if (BlackPusaIsDead == false)
                        {
                            AnimationNumber = 8;
                        }
                        else
                        {
                            cout << "Cant kill a dead being!!!" << endl;
                            BlackPusaIsDeadLimiter = true;
                        }
                    }
                    else
                    {
                        if (YellowPusaIsDead == false)
                        {
                            AnimationNumber = 8;
                        }
                        else
                        {
                            cout << "Cant kill a dead being!!!" << endl;
                            YellowPusaIsDeadLimiter = true;
                        }
                    }
                    FurinaAnimation.AnimationReset(FurinaAnimation.CurrentImage);
                }
            }
            if (FurinaAnimation.CurrentImage.y == 5)
            {
                if (FurinaAnimation.CurrentImage.x == 0)
                {
                    FurinaAnimation.AnimationReset(FurinaAnimation.CurrentImage);

                }
            }
        }


        if (AnimationNumber == 8)
        {
            MainUltimateAnimation.UpdateGridAfterUltimate(0, DeltaTime, sf::Vector2u(10, 11));//add 1 to x

            MainBackground_After.setTextureRect(MainUltimateAnimation.UvRect);

            Window.clear();
            Window.draw(MainBackground_After);

            if (MainUltimateAnimation.CurrentImage.y == 7)
            {

                if (MainUltimateAnimation.CurrentImage.x >= 8)
                {
                    if (AttackRight == true)
                    {
                        Window.draw(EnemyCharacter1_After);
                    }
                    else
                    {
                        Window.draw(EnemyCharacter2_After);
                    }
             
                }
            }

            if (MainUltimateAnimation.CurrentImage.y >= 8)
            {

                if (MainUltimateAnimation.CurrentImage.x >= 0)
                {
                    if (AttackRight == true)
                    {
                        Window.draw(EnemyCharacter1_After);
                    }
                    else
                    {
                        Window.draw(EnemyCharacter2_After);
                    }
                }
            }

            if (MainUltimateAnimation.CurrentImage.y == 8)
            {

                if (MainUltimateAnimation.CurrentImage.x >= 8)
                {
                    Explosion.UpdateGridAfterUltimate(0, DeltaTime, sf::Vector2u(0, 5));
                    Effect_After.setTextureRect(Explosion.UvRect);
                    Window.draw(Effect_After);
                }
            }

            if (MainUltimateAnimation.CurrentImage.y >= 9)
            {

                if (MainUltimateAnimation.CurrentImage.x >= 0)
                {
                    Explosion.UpdateGridAfterUltimate(0, DeltaTime, sf::Vector2u(0, 5));
                    Effect_After.setTextureRect(Explosion.UvRect);
                    Window.draw(Effect_After);
                }
            }

            if (MainUltimateAnimation.CurrentImage.y >= 11)
            {

                if (MainUltimateAnimation.CurrentImage.x >= 9)
                {
                    Explosion.UpdateGridAfterUltimate(0, DeltaTime, sf::Vector2u(1, 5));
                    Explosion.AnimationReset(Explosion.CurrentImage);
                    AnimationNumber = 2;
                    characters[CurrentTurn].EnergyInside = 0;
                    CharacterDamage = CalculateBaseDamage(1.8, characters[CurrentTurn].Attack);
                    IsCritAttack = CritIdentifier(characters[CurrentTurn].CritRate);
                    if (IsCritAttack)
                    {
                        CharacterDamage *= 1 + characters[CurrentTurn].CritDamage;
                    }
                    if (AttackRight == true)
                    {
                        for (auto& character : characters)
                        {
                            if (character.Name == "Maxwell Cat")
                            {
                                character.Health = ApplyDamageAndLimiter(character.Health, CharacterDamage, character.IsDead);
                                if (character.IsDead == true)
                                {
                                    BlackPusaIsDead = true;
                                }
                                if (character.Health <= 0)
                                {
                                    characters[CurrentTurn].EnergyInside = AddAndDisplayEnergyPointAndLimiter(characters[CurrentTurn].EnergyInside, characters[CurrentTurn].ERRate, 10);
                                }
                                SkillPoint = SubtractAndDisplaySkillPointAndLimiter(SkillPoint);
                                characters[CurrentTurn].EnergyInside = AddAndDisplayEnergyPointAndLimiter(characters[CurrentTurn].EnergyInside, characters[CurrentTurn].ERRate, 20);

                                //HpDisplayed = false;
                                //system("cls"); COMMENT
                                if (HpDisplayed == false)
                                {
                                    for (auto& charactering : characters)
                                    {
                                        cout << charactering.Name << " Remaining HP: " << charactering.Health << "\n";
                                    }
                                    HpDisplayed = true;
                                }

                            }
                        }
                    }
                    else
                    {
                        for (auto& character : characters)
                        {
                            if (character.Name == "Chonker Cat")
                            {
                                character.Health = ApplyDamageAndLimiter(character.Health, CharacterDamage, character.IsDead);
                                if (character.IsDead == true)
                                {
                                    YellowPusaIsDead = true;
                                }
                                if (character.Health <= 0)
                                {
                                    characters[CurrentTurn].EnergyInside = AddAndDisplayEnergyPointAndLimiter(character.EnergyInside, character.ERRate, 10);
                                }
                                SkillPoint = SubtractAndDisplaySkillPointAndLimiter(SkillPoint);
                                characters[CurrentTurn].EnergyInside = AddAndDisplayEnergyPointAndLimiter(characters[CurrentTurn].EnergyInside, characters[CurrentTurn].ERRate, 20);

                                //HpDisplayed = false;
                                //system("cls"); COMMENT
                                if (HpDisplayed == false)
                                {
                                    for (auto& charactering : characters)
                                    {
                                        cout << charactering.Name << " Remaining HP: " << charactering.Health << "\n";
                                    }
                                    HpDisplayed = true;
                                }
                            }
                        }
                    }

                    CurrentTurn++;
                    if (CurrentTurn == 3)
                    {
                        CurrentTurn = 0;
                    }

                    if (characters[CurrentTurn].Player == 1)
                    {
                        AnimationNumber = 2;
                    }
                    else
                    {
                        if (characters[CurrentTurn].IsEnemy1 == 1)
                        {
                            AnimationNumber = 5;
                        }
                        else
                        {
                            AnimationNumber = 6;
                        }
                    }

                }
            }
            Window.display();
        }
        //}
        /*else
        {
            CurrentTurn++;
            if (CurrentTurn == 3)
            {
                CurrentTurn = 0;
            }

            if (characters[CurrentTurn].Player == 1)
            {
                AnimationNumber = 2;
            }
            else
            {
                if (characters[CurrentTurn].IsEnemy1 == 1)
                {
                    cout << "Guba imo code brad" << endl;
                }
                else
                {
                    AnimationNumber = 5;
                }
            }
            }*/


            //}

        }
        return 0;
    }


int CalculateActionValue(int ActionGauge, int Speed)
{
    return ceil(ActionGauge / Speed);
}
double EnergyCalculator(double EnergyRegenerationRate, int BaseEnergy)
{
    double add = 1 + EnergyRegenerationRate;
    return add * BaseEnergy;
}
double CalculateBaseDamage(double DamagePercentage, double ScalingAttribute) //scalingAttribute = attack
{
    return DamagePercentage * ScalingAttribute;
}
void ActionValueSorter(vector<Character>& characters)
{
    sort(characters.begin(), characters.end(),
        [](const Character& a, const Character& b) {
            return a.ActionValue < b.ActionValue;
        });
}
void DisplayUI(int SkillPoint, float CharacterEnergy)
{
    cout << "\nChoose your attack\n";
    cout << "1. Basic Attack\n";
    cout << "2. Skill\n";
    cout << "3. Ultimate\n";
    cout << "Skill Point Remaining: " << SkillPoint << "\n";
    cout << "Energy: " << CharacterEnergy << " / 120\n"; //EnergyCap is 120
    cout << "Who do you want to attack?: ";

}
int ApplyDamageAndLimiter(int CharacterHealth, int Damage, bool& IsDead)
{
    CharacterHealth -= Damage;
    if (CharacterHealth <= 0)
    {
        CharacterHealth = 0;
        IsDead = true;
    }
    return CharacterHealth;

}
float AddAndDisplayEnergyPointAndLimiter(float MainCharacterEnergy, float MainCharacterEnergyRate, int EnergyRecharge)
{
    MainCharacterEnergy += EnergyCalculator(MainCharacterEnergyRate, EnergyRecharge);
    //ceil(MainCharacterEnergy);
    if (MainCharacterEnergy >= 119.45)
    {
        MainCharacterEnergy = 120;
    }
    cout << "Current Energy: " << MainCharacterEnergy << " / 120\n\n";
    return MainCharacterEnergy;
}
void DisplayDamageDealt(string FirstCharacterName, int FirstCharacterDamage, string SecondCharacterName, int SecondCharacterHealth, bool& IfCrit)
{
    if (IfCrit)
    {
        cout << "\n" << FirstCharacterName << " Dealt CRITICAL!!!!! damage: " << FirstCharacterDamage << " to " << SecondCharacterName
            << "\nRemaining Health of " << SecondCharacterName << ": " << SecondCharacterHealth << "\n\n";
    }

    else
    {
        cout << "\n" << FirstCharacterName << " Dealt damage: " << FirstCharacterDamage << " to " << SecondCharacterName
            << "\nRemaining Health of " << SecondCharacterName << ": " << SecondCharacterHealth << "\n\n";
    }

    IfCrit = false;
}
int AddAndDisplaySkillPointAndLimiter(int SkillPoint)
{
    SkillPoint += 1;
    if (SkillPoint >= 5)
    {
        SkillPoint = 5;
    }
    if (SkillPoint <= 0)
    {
        SkillPoint = 0;
    }
    cout << "Remaining Skill Point: " << SkillPoint << "\n\n\n";
    return SkillPoint;
}
int SubtractAndDisplaySkillPointAndLimiter(int SkillPoint)
{
    SkillPoint -= 1;
    if (SkillPoint >= 5)
    {
        SkillPoint = 5;
    }
    if (SkillPoint <= 0)
    {
        SkillPoint = 0;
    }
    cout << "Remaining Skill Point: " << SkillPoint << "\n\n\n";
    return SkillPoint;
}
bool CritIdentifier(float CritRate)
{
    float Random = static_cast<float>(rand() % 101);
    if (Random <= CritRate)
    {
        return true;
    }
    return false;
}
