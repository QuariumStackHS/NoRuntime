#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include "3rd_Party/SHA1.hpp"
#include <string>

using namespace std;

string removeIdentifier(string K)
{
    string Out = "";
    for (int i = 1; i < K.size(); i++)
        Out.push_back(K[i]);
    return Out;
}
string spliters = " ,\n\t";
bool spliter(char K)
{
    for (int i = 0; i < spliters.size(); i++)
    {
        if (spliters[i] == K)
            return 1;
    }
    return 0;
}
string removeL(string k)
{
    string K = "";
    for (int i = 0; i < k.size() - 1; i++)
        K.push_back(k[i]);
    return K;
}
string removeF(string k)
{
    string K = "";
    for (int i = 1; i < k.size(); i++)
        K.push_back(k[i]);
    return K;
}

bool strcmp(string a, string b)
{
    if (a.size() != b.size())
        return 0;
    for (int i = 0; i < a.size(); i++)
        if (a[i] != b[i])
            return 0;
    return 1;
}
int MatchSHA1File(std::string StorageName, std::string Filename)
{
    SHA1 Creaper = SHA1();
    std::string CurrentKey = Creaper.from_file(Filename);
    std::ifstream StorageFile(StorageName);
    std::stringstream ss;
    ss << StorageFile.rdbuf();
    StorageFile.close();
    if (strcmp(ss.str(), CurrentKey))
    {
        return 1;
    }
    else
    {
        std::ofstream StorageFileOverwrite(StorageName);
        StorageFileOverwrite << CurrentKey;
        StorageFileOverwrite.close();
        return 0;
    }
}
namespace TokensEnums
{

    enum Token
    {
        MOV, // mov left to right
        INT, // call interupt
        PUSH,
        POP,
        PUSHA,
        POPA,

        PUT,

        ConstStr,

        Label, // id of label definition
        Value,
        REGISTER, // any Know Register will get this id
        ASPTR,    // Token that precise to use the left value as a pointer
        INC,      // increment incrementator
        DEC,      // decrement Incrementator
        JMP,
        JE,
        JNE, // jump without condition
        JME, // jump if Mathout>MathLeft more
        JLE, // jump if Mathout<MathLeft
        Addr,
        RETURN,
        // operators
        ADD,
        MUL,
        DIV,
        SUB,

        OR,
        AND,
        XOR,
        EQ,

    };
    enum DefinedRegister
    {
        MATHLEFT,  // ML
        MATHRIGHT, // MR
        MATHOUT,   // MO
        CODEPTR,   // CPR
        COUNTER,   // CTR
        NBDR,      // NumBer Defined Register
    };
    class Register
    {
    protected:
        class RegisterData
        {
        protected:
            unsigned int TOKEN;
            int64_t Value;
            RegisterData(Token ID, int64_t V)
            {
                this->TOKEN = ID, this->Value = V;
            }
            friend class Register;
        };
        friend class Interpreter;
        vector<RegisterData *> StackOfthisRegisterType;

    public:
        Register(unsigned int TOken)
        {
            push(new RegisterData((Token)TOken, 0));
        }
        void push(RegisterData *ADDR)
        {
            this->StackOfthisRegisterType.push_back(ADDR);
        }
        int64_t Get()
        {
            return this->StackOfthisRegisterType[this->StackOfthisRegisterType.size() - 1]->Value;
        }
        void Set(int64_t K)
        {
            this->StackOfthisRegisterType[this->StackOfthisRegisterType.size() - 1]->Value = K;
        }
        void pop()
        {
            this->StackOfthisRegisterType.pop_back();
        }
    };

    enum Interupts
    {
        PRINT,
        CIN,

    };
    class Interpreter
    {
    public:
        class ExecutableToken
        {
        public:
            int ID;
            int64_t Data;
            string StrData;
            ExecutableToken(int Id, int DAta, string STRData)
            {
                ID = Id, Data = DAta, StrData = STRData;
            }
        };

        map<int, int> DataRam;

        map<int, Register *> RegisterIDToInstance;
        map<string, int> RegisterNameToID;
        map<string, int> LabeltoStart;
        vector<ExecutableToken *> ProgrameCS;
        vector<int>CallStack;
        void INITRegister(string name, DefinedRegister RID)
        {
            RegisterIDToInstance[RID] = new Register(RID);
            RegisterNameToID[name] = RID;
        }
        void INITRegisters()
        {
            this->INITRegister("ML", MATHLEFT);
            this->INITRegister("MR", MATHRIGHT);
            this->INITRegister("MO", MATHOUT);
            this->INITRegister("CTR", COUNTER);
        }
        void DefineLabel(string name, int start)
        {
            LabeltoStart[name] = start;
        }
        int Execute(string FunctionStart = "Start")
        {
            int Temp;
            bool asptr = 0;
            for (int i = this->LabeltoStart[FunctionStart]; i < this->ProgrameCS.size(); i++)
            {
#define Arg_0 this->ProgrameCS[i]
#define Arg_1 this->ProgrameCS[i + 1]
#define Arg_2 this->ProgrameCS[i + 2]
#define Arg_3 this->ProgrameCS[i + 3]
#define Arg_4 this->ProgrameCS[i + 4]
                // cout<<Arg_0->ID<<endl;
                switch (Arg_0->ID)
                {
                case Token::MOV:
                    if (Arg_1->ID == Addr)
                    {
                        Temp = this->DataRam[Arg_1->Data];
                    }
                    else if (Arg_1->ID == Token::REGISTER)
                    {
                        Temp = this->RegisterIDToInstance[Arg_1->Data]->Get();
                    }
                    else if (Arg_1->ID == Token::Value)
                    {
                        Temp = Arg_1->Data;
                    }
                    if (Arg_2->ID == Token::ASPTR)
                    {
                        Temp = this->DataRam[Temp];
                    }
                    if (Arg_3->ID == Token::ASPTR || Arg_4->ID == Token::ASPTR)
                    {
                        asptr = 1;
                    }
                    if (Arg_2->ID == Token::REGISTER)
                    {
                        if (asptr)
                            this->DataRam[this->RegisterIDToInstance[Arg_2->Data]->Get()] = (Temp), asptr = 0;
                        else
                            this->RegisterIDToInstance[Arg_2->Data]->Set(Temp);
                    }
                    else if (Arg_2->ID == Token::Addr)
                    {
                        if (asptr)
                            this->DataRam[this->DataRam[Arg_2->Data]] = (Temp), asptr = 0;
                        else
                            this->DataRam[Arg_2->Data] = (Temp);
                    }
                    break;
                case Token::JMP:
                    this->CallStack.push_back(i+1);
                    i=this->LabeltoStart[Arg_1->StrData];
                    break;
                case Token::RETURN:
                    i=this->CallStack[this->CallStack.size()-1];
                    this->CallStack.pop_back();
                    break;
                case Token::PUT:
                    cout << this->DataRam[Arg_1->Data] << endl;
                    break;
                default:
                    break;
                }
            }
            // cout<<this->LabeltoStart["Start"]<<endl;
        }
        int Compiler(string SRC)
        {
            SRC.push_back('\n');
            string substring = "";
            for (int i = 0; i < SRC.size(); i++)
            {
                if (spliter(SRC[i]))
                {
#define KeyWordCreate(ID, WORD)  \
    if (strcmp(WORD, substring)) \
    ProgrameCS.push_back(new ExecutableToken(ID, 0, ""))

#define RegisterCreate(WORD, ID)      \
    else if (strcmp(WORD, substring)) \
        ProgrameCS.push_back(new ExecutableToken(REGISTER, ID, ""))

                    KeyWordCreate(Token::OR, "or");
                    else KeyWordCreate(Token::AND, "and");
                    else KeyWordCreate(Token::MOV, "mov");
                    else KeyWordCreate(Token::PUT, "put");
                    else KeyWordCreate(Token::ASPTR, "->");
                    RegisterCreate("ML", DefinedRegister::MATHLEFT);
                    RegisterCreate("MR", DefinedRegister::MATHRIGHT);
                    RegisterCreate("MO", DefinedRegister::MATHOUT);
                    RegisterCreate("CTR", DefinedRegister::COUNTER);

                    else if (substring[0] == '$')
                    {
                        try
                        {
                            ProgrameCS.push_back(new ExecutableToken(Token::Value, stoi(removeF(substring)), ""));
                        }
                        catch (...)
                        {
                            ProgrameCS.push_back(new ExecutableToken(Token::ConstStr, 0, removeF(substring)));
                        }
                    }
                    else if (substring[0] == '&')
                    {
                        try
                        {
                            ProgrameCS.push_back(new ExecutableToken(Token::Addr, stoi(removeF(substring)), ""));
                        }
                        catch (...)
                        {
                            cout << "unknown Address: -> " << removeF(substring) << endl;
                        }
                    }
                    else if (substring[substring.size() - 1] == ':')
                    {
                        ProgrameCS.push_back(new ExecutableToken(Token::Label, 0, removeL(substring)));
                        this->LabeltoStart[removeL(substring)] = ProgrameCS.size();
                    }
                    else if (substring[0] == '|')
                    {
                        ifstream NewObj(removeF(substring));
                        stringstream ss;
                        ss << NewObj.rdbuf();
                        NewObj.close();
                        this->Compiler(ss.str());
                        ss.clear();
                    }
                    else
                    {
                        if (!strcmp(substring, ""))
                            cout << "Unkown substring: " << substring << endl;
                    }

                    substring = "";
                }
                else
                {
                    substring.push_back(SRC[i]);
                }
            }
            for (int i = 0; i < 4; i++)
            {
                this->ProgrameCS.push_back(new ExecutableToken(999, 0, ""));
            }
            return 0;
        }
        Interpreter()
        {
        }
    };
};

int main(int argc, char **argv)
{
    
    
    TokensEnums::Interpreter *Int = new TokensEnums::Interpreter();
    Int->INITRegisters();
    ifstream SourceFile(argv[1]);
    stringstream ss;
    ss << SourceFile.rdbuf();
    Int->Compiler(ss.str());
    Int->Execute();
    getchar();
}