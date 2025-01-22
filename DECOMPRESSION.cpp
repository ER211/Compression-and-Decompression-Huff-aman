#include<iostream>
#include<string>
#include<fstream>
using namespace std;

class cnode {
public:
    string ltr;
    string bin;
    cnode* pnext;
};

class clist {
public:
    cnode* phead;
    cnode* ptail;

    clist() : phead(NULL), ptail(NULL) {}

    ~clist()
    {
        while (phead != NULL) 
        {
            cnode* temp = phead;
            phead = phead->pnext;
            delete temp;
        }
    }

    void attach(cnode* pnn)
    {
        if (phead == NULL)
        {
            phead = ptail = pnn;
        }
        else 
        {
            ptail->pnext = pnn;
            ptail = pnn;
        }
    }
};

void decompress(const string& huffmanFilePath, const string& compressedFilePath, const string& outputFilePath) 
{
    ifstream readfile(huffmanFilePath, ios::binary);
    ifstream readcompressedfile(compressedFilePath, ios::binary);
    ofstream outfile(outputFilePath, ios::binary);

    if (!readfile.is_open() || !readcompressedfile.is_open() || !outfile.is_open())
    {
        cerr << "Error opening files!" << endl;
        return;
    }

    clist huffcode;
    cnode* pnn = NULL;
    char ch;

    while (readfile.read(&ch, 1))
    {
        pnn = new cnode();
        pnn->ltr = ch;
        pnn->pnext = NULL;
        readfile.read(&ch, 1);
        int codeLength = (int)ch;

        for (int i = 0; i < codeLength; i++) 
        {
            readfile.read(&ch, 1);
            pnn->bin += ch;
        }
        huffcode.attach(pnn);
    }

    string currentBinary;
    char byte;
    while (readcompressedfile.read(&byte, 1)) 
    {
        for (int bit = 7; bit >= 0; --bit) 
        {
            if ((byte >> bit) & 1) 
            {
                currentBinary += '1';
            }
            else {
                currentBinary += '0';
            }


            cnode* ptrave = huffcode.phead;
            while (ptrave != NULL) {
                if (currentBinary == ptrave->bin)
                {
                    char firstChar = ptrave->ltr[0]; 
                    outfile.write(&firstChar, 1);   
                    currentBinary.clear();
                    break;
                }
                ptrave = ptrave->pnext;
            }
        }
    }

    readfile.close();
    readcompressedfile.close();
    outfile.close();
}

int main() {
    string huffmanFilePath = "huffmancode.bin";
    string compressedFilePath = "compressedfile.bin";
    string outputFilePath = "decompressedfile.bmp";

    decompress(huffmanFilePath, compressedFilePath, outputFilePath);

    cout << "Decompression completed!" << endl;
    return 0;
}
