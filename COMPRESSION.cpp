#include <iostream>
#include <string>
#include <fstream>
using namespace std;

class trnode {
public:
    int vl;
    string lt;
    trnode* pright;
    trnode* pleft;
};

class cnode {
public:
    string ltr;
    int val;
    cnode* pnext;
    trnode* pdown;
    string bin = "";
};

class clist {
public:
    cnode* phead;
    cnode* ptail;

    clist() {
        phead = NULL;
        ptail = NULL;
    }
    ~clist() {
        while (phead != NULL) {
            cnode* ptrave = phead;
            phead = phead->pnext;
            delete ptrave;
        }
    }

    void sortattach(cnode* pnn) 
    {
        if (phead == NULL || pnn->val <= phead->val)
        {
            pnn->pnext = phead;
            phead = pnn;
            if (ptail == NULL) {
                ptail = pnn;
            }
        }
        else {
            cnode* ptrave = phead;
            while (ptrave->pnext != NULL && ptrave->pnext->val < pnn->val) 
            {
                ptrave = ptrave->pnext;
            }
            pnn->pnext = ptrave->pnext;
            ptrave->pnext = pnn;
            if (pnn->pnext == NULL) 
            {
                ptail = pnn;
            }
        }
    }
};

class sbtnode 
{
public:
    trnode* proot;

    sbtnode() 
    {
        proot = NULL;
    }

    void insert(trnode* pnn)
    {
        if (proot == NULL) 
        {
            proot = pnn;
        }
        else
        {
            if (pnn->pleft != NULL && pnn->pleft->vl == proot->vl) 
            {
                pnn->pleft = proot;
                proot = pnn;
            }
            else {
                pnn->pright = proot;
                proot = pnn;
            }
        }
    }
};

void generatebincodes(trnode* root, string bincode, clist& huffmanencode)
{
    if (root == NULL) return;
    if (root->pleft == NULL && root->pright == NULL) 
    {
        cnode* tempnode = new cnode();
        tempnode->ltr = root->lt;
        tempnode->val = root->vl;
        tempnode->bin = bincode;
        tempnode->pnext = NULL;
        huffmanencode.sortattach(tempnode);
    }
    generatebincodes(root->pright, bincode + "0", huffmanencode);
    generatebincodes(root->pleft, bincode + "1", huffmanencode);
}

void disp(clist& l) 
{
    cnode* ptrave = l.phead;
    while (ptrave != NULL) 
    {
        cout << ptrave->ltr << " , " << ptrave->val;
        if (ptrave->bin != "") 
        {
            cout << " -> " << ptrave->bin << endl;
        }
        else {
            cout << endl;
        }
        ptrave = ptrave->pnext;
    }
}

int main() {
    clist L;
    cnode* ptrave = NULL, * pnn = NULL;

    ifstream readfile("testread.bmp", ifstream::binary);
    ofstream outfile("compressedfile.bin", ofstream::binary);
    ofstream outfile2("huffmancode.bin", ofstream::binary);

    if (!readfile)
    {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    readfile.seekg(0, readfile.end);
    long long lenread = readfile.tellg();
    cout << "File size: " << lenread << " bytes" << endl;
    readfile.seekg(0, readfile.beg);

    char* read = new char[lenread];
    char* dc = new char[lenread];
    readfile.read(read, lenread);

    int freq[256] = { 0 };
    for (long long i = 0; i < lenread; i++) 
    {
        freq[(unsigned char)read[i]]++;
    }

    for (int i = 0; i < 256; i++) 
    {
        if (freq[i] > 0) 
        {
            pnn = new cnode();
            pnn->ltr = string(1, (char)i);
            pnn->val = freq[i];
            pnn->pnext = NULL;
            L.sortattach(pnn);
        }
    }

    sbtnode T;
    while (L.phead != NULL && L.phead->pnext != NULL) 
    {
        cnode* left = L.phead;
        cnode* right = L.phead->pnext;
        L.phead = right->pnext;

        trnode* temproot = new trnode();
        temproot->lt = left->ltr + right->ltr;
        temproot->vl = left->val + right->val;

        if (left->pdown != NULL) 
        {
            temproot->pleft = left->pdown;
        }
        else {
            temproot->pleft = new trnode{ left->val, left->ltr, NULL, NULL };
        }

        if (right->pdown != NULL) 
        {
            temproot->pright = right->pdown;
        }
        else
        {
            temproot->pright = new trnode{ right->val, right->ltr, NULL, NULL };
        }


        pnn = new cnode();
        pnn->ltr = temproot->lt;
        pnn->val = temproot->vl;
        pnn->pnext = NULL;
        pnn->pdown = temproot;
        L.sortattach(pnn);

        delete left, right;
    }

    T.insert(L.phead->pdown);

    clist huffcode;
    generatebincodes(T.proot, "", huffcode);

    cout << "Huffman Encodes:\n";
    disp(huffcode);

    ptrave = huffcode.phead;
    while (ptrave != NULL)
    {
        outfile2.write(ptrave->ltr.c_str(), 1);
        char num = (ptrave->bin.length());
        outfile2.write(&num, 1);
        outfile2.write(ptrave->bin.c_str(), ptrave->bin.length());
        ptrave = ptrave->pnext;
    }

    char temp = 0;
    char m = 1;
    int ct = 0;
    long long k = 0;
    for (long long i = 0; i < lenread; i++) 
    {
        ptrave = huffcode.phead;
        while (ptrave != NULL) 
        {
            if (ptrave->ltr[0] != read[i])
            {
                ptrave = ptrave->pnext;
            }
            else {
                for (int p = 0; p < ptrave->bin.length(); p++)
                {
                    if (ct == 8) 
                    {
                        dc[k] = temp;
                        k++;
                        ct = 0;
                        temp = 0;
                    }
                    if (ptrave->bin[p] == '1') 
                    {
                        temp |= (m << (7 - ct));
                    }
                    ct++;
                }
                break;
            }
        }
    }
    if (ct > 0)
    {
        dc[k] = temp;
        k++;
    }

    outfile.write(dc, k);
    delete[] read;
    delete[] dc;

    readfile.close();
    outfile.close();
    outfile2.close();
    return 0;
}
