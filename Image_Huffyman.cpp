// Image Compression
#include<bits/stdc++.h>
#include<iostream>
#include <cstdlib>
#include <string>
#include<graphics.h> // header
#include<conio.h>
#include<fstream>
#include<stdio.h>
using namespace std;

//calculates word length
int codelen(char* code)
{
    int length = 0;
    while (*(code + length) != '\0')
    length++;
    return length;
}

// concatenates the words
void stringconcat(char* string1, char* code, char add)
{
    int i = 0;
    while (*(code + i) != '\0')
    {
        *(string1 + i) = *(code + i);
        i++;
    }
    if (add != '2')
    {
        string1[i] = add;
        string1[i+1] = '\0';
    }
    else
        string1[i] = '\0';
}

//  fibonacci series number
int fib(int n)
{
  int a = 0, b = 1, c;
  if( n == 0)
    return a;
  for (int i = 2; i <= n; i++)
  {
     c = a + b;
     a = b;
     b = c;
  }
  return b;
}

//main function
int main()
{
    int i, j;
    char filename[] = "1234.bmp"; // file to be compressed (bmp file only)
    int data = 0, move_by, bpp = 0, width, height;
    long bmpsize = 0, bmpdataoff = 0;
    int** image;
    int temp = 0;

    // Reading the BMP File
    FILE* image_file;
    //image_file.open("filename",ios::in|ios::out|ios::binary);
    image_file = fopen(filename, "rb");
    if (!image_file)
    {
        printf("Error Opening File!!");
        exit(1);
    }
    else
    {
        move_by = 0; // to set position of pointer i.e. get or put pointer
        move_by = 2; // set to 2 to get bmp size
       // image_file.seekp(move_by,ios_base::beg);
        fseek(image_file, move_by, SEEK_SET);
        //bmpsize = getfilesize(image_file);
        fread(&bmpsize, 4, 1, image_file); // Getting size of BMP File

        move_by = 10;// Getting offset where the pixel array starts
        //image_file.seekp(move_by,ios_base::beg);
        fseek(image_file, move_by, SEEK_SET);
        // Bitmap data offset
        fread(&bmpdataoff, 4, 1, image_file);

        // Getting height and width of the image
        // Width is stored at offset 18 and
        // height at offset 22, each of 4 bytes
        fseek(image_file, 18, SEEK_SET);

        fread(&width, 4, 1, image_file);

        fread(&height, 4, 1, image_file);

        // Number of bits per pixel
        fseek(image_file, 2, SEEK_CUR);

        fread(&bpp, 2, 1, image_file);

        // Setting offset to start of pixel data
        fseek(image_file, bmpdataoff, SEEK_SET);

        // Creating Image array
        image=new int *[height];
        for(int count1=0;count1<height;count1++)
        {

            image[count1]=new int[width];
        }

        int numbytes = (bmpsize - bmpdataoff) / 3; // Number of bytes in image pixel array

        for (i = 0; i < height; i++) // Reading bmp file into image array
        {
            for (j = 0; j < width; j++)
            {
                fread(&temp, 3, 1, image_file);
                temp = temp & 0x0000FF; // Image is 24 bit image
                image[i][j] = temp;
            }
        }
    }
      fclose(image_file); //closing image file
      cout << "   ---------------------------IMAGE COMPRESSION USING HUFFMAN CODING---------------------------"<<endl<<endl<<endl;

      cout << "   --------- Intensity array of Image --------\n\n";
// loop to print intensity of each pixel of image
       for (i = 0; i < height; i++)
        {
            cout <<"\t";
            for (j = 0; j < width; j++)
            {
                cout << image[i][j]<<"\t";
            }
           cout <<endl;
        }
        cout << "\n\n";

    // Finding the probability of occurence
    int histogram[256];
    for (i = 0; i < 256; i++)
        histogram[i] = 0;
    for (i = 0; i < height; i++)
        for (j = 0; j < width; j++)
            histogram[image[i][j]] += 1;

    // Finding number of non-zero occurences
    int nodes = 0;
    for (i = 0; i < 256; i++)
        if (histogram[i] != 0)
            nodes += 1;

    // Calculating minimum probability
    float p = 1.0, ptemp;
    for (i = 0; i < 256; i++)
    {
        ptemp = (histogram[i] / (float)(height * width));
        if (ptemp > 0 && ptemp <= p)
            p = ptemp;
    }

    // Calculating max length
    // of code word
    i = 0;
    while ((1 / p) > fib(i))
        i++;
    const int maxcodelen = i - 3;

    // Defining Structures pixfreq
    struct pixfreq
    {
        int pix, larrloc, rarrloc;
        float freq;
        struct pixfreq *left, *right;
        char code[100];
    };

    // Defining Structures
    // huffcode
    struct huffcode
    {
        int pix, array_location;
        float freq;
    };

    // Declaring structs
    int totalnodes = 2 * nodes - 1;
    struct pixfreq* pix_freq = new pixfreq[totalnodes];
    struct huffcode* huffcodes = new huffcode[nodes];

    /*pix_freq = (struct pixfreq*)malloc(sizeof(struct pixfreq) * totalnodes);
    huffcodes = (struct huffcode*)malloc(sizeof(struct huffcode) * nodes);*/

    // Initializing
    j = 0;
    int totalpix = height * width;
    float tempprob;
    for (i = 0; i < 256; i++)
    {
        if (histogram[i] != 0)
        {

            // pixel intensity value
            huffcodes[j].pix = i;
            pix_freq[j].pix = i;
            huffcodes[j].array_location = j;// location of the node in the pix_freq array
            // probability of occurrence
            tempprob = (float)histogram[i] / (float)totalpix;
            pix_freq[j].freq = tempprob;
            huffcodes[j].freq = tempprob;

            // Declaring the child of leaf node as NULL pointer
            pix_freq[j].left = NULL;
            pix_freq[j].right = NULL;

            // initializing the code word as end of line
            pix_freq[j].code[0] = '\0';
            j++;
        }
    }

    // Sorting the histogram
    struct huffcode temphuff;
    // Sorting w.r.t probability of occurences in n2 complexity
    for (i = 0; i < nodes; i++)
    {
        for (j = i + 1; j < nodes; j++)
        {
            if (huffcodes[i].freq < huffcodes[j].freq)
            {
                temphuff = huffcodes[i];
                huffcodes[i] = huffcodes[j];
                huffcodes[j] = temphuff;
            }
        }
    }

    // Building Huffman Tree
    float sumprob;
    int sumpix;
    int n = 0, k = 0;
    int nextnode = nodes;

    // Since total number of
    // nodes in Huffman Tree
    // is 2*nodes-1
    while (n < nodes - 1)
    {

        // Adding the lowest two probabilities
        sumprob = huffcodes[nodes - n - 1].freq + huffcodes[nodes - n - 2].freq;
        sumpix = huffcodes[nodes - n - 1].pix + huffcodes[nodes - n - 2].pix;

        // Appending to the pix_freq Array
        pix_freq[nextnode].pix = sumpix;
        pix_freq[nextnode].freq = sumprob;
        pix_freq[nextnode].left = &pix_freq[huffcodes[nodes - n - 2].array_location];
        pix_freq[nextnode].right = &pix_freq[huffcodes[nodes - n - 1].array_location];
        pix_freq[nextnode].code[0] = '\0';
        i = 0;

        // Sorting and Updating the
        // huffcodes array simultaneously
        // New position of the combined node
        while (sumprob <= huffcodes[i].freq)
              i++;

        // Inserting the new node
        // in the huffcodes array
        for (k = nodes; k >= 0; k--)
        {
            if (k == i)
            {
                huffcodes[k].pix = sumpix;
                huffcodes[k].freq = sumprob;
                huffcodes[k].array_location = nextnode;
            }
            else if (k > i)

                // Shifting the nodes below
                // the new node by 1
                // For inserting the new node
                // at the updated position k
                huffcodes[k] = huffcodes[k - 1];

        }
        n += 1;
        nextnode += 1;
    }

    // Assigning Code through
    // backtracking
    char left = '0';
    char right = '1';
    int index;
    for (i = totalnodes - 1; i >= nodes; i--)
    {
        if (pix_freq[i].left != NULL)
            stringconcat(pix_freq[i].left->code, pix_freq[i].code, left);
        if (pix_freq[i].right != NULL)
            stringconcat(pix_freq[i].right->code, pix_freq[i].code, right);
    }

    // Encode the Image
    int pix_val;
    int l;
    // Writing the Huffman encoded
    // Image into a text file
    ofstream filehb;

    filehb.open("encoded_image_binary_file.txt",ios::binary|ios::trunc);
    for (i = 0; i < height; i++)
        for (j = 0; j < width; j++)
        {
            pix_val = image[i][j];
            for (l = 0; l < nodes; l++)
                if (pix_val == pix_freq[l].pix)
                {
                     filehb<<pix_freq[l].code;
                    filehb<<"\t";
                }
                filehb<<"\n";
        }
        filehb.close();
    ofstream file123;
    file123.open("encoded_image123.txt", ios::out|ios::trunc);
    while(!file123){file123.open("encoded_image123.txt", ios::out|ios::trunc);}
    ofstream file1234;
    file1234.open("yo13345.txt", ios::out|ios::trunc);
    cout<<"y";
     for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
                file1234<<image[i][j];
                    file1234<<"\t";
        }
         file1234<<"\n";
    }
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            pix_val = image[i][j];
            for (l = 0; l < nodes; l++)
                if (pix_val == pix_freq[l].pix)
                    file123<<pix_freq[l].code;
                    file123<<"\t";
        }
file123<<"\n";
    }
    file1234.close();
    file123.close();
    // Printing Huffman Codes
    printf("   ---------- Huffmann Codes ----------\n\n");
    printf("  pixel values ->   Code\n\n");
    for (i = 0; i < nodes; i++) {
	if (snprintf(NULL, 0, "%d", pix_freq[i].pix) == 2)
            printf("    %d       -> %s\n", pix_freq[i].pix, pix_freq[i].code);
        else
            printf("    %d      -> %s\n", pix_freq[i].pix, pix_freq[i].code);
    }

    // Calculating Average Bit Length
    float avgbitnum = 0;
    for (i = 0; i < nodes; i++)
        avgbitnum += pix_freq[i].freq * codelen(pix_freq[i].code);
    printf("\n   Average number of bits :: %0.3f\n\n", avgbitnum);


    map<string,int> map1;
    fstream file12;
    fstream file13;
    string word, t, q, filename12,filename13,word1;
    filename12 = "encoded_image123.txt";
    filename13 = "yo13345.txt";
    file12.open(filename12.c_str());
    file13.open(filename13.c_str());
    while (file12 >> word&&file13>>word1)
    {
        int x;
        x = std::atoi (word1.c_str());
        map1[word]=x;
    }
    map<string, int>::iterator itr;
  /*  cout << "\nThe map gquiz1 is : \n";
    cout << "\tKEY\tELEMENT\n";
   for (itr = map1.begin(); itr != map1.end(); ++itr) {
        cout << '\t' << itr->first
             << '\t' << itr->second << '\n';
    }
    printf("yo");
    cout << endl;*/

    file12.close();
    file13.close();
    //file.close();
    //file1.close();*/
   int temp=0,temp1=0;
    int var=0,var1=0;
    fstream file14;
    string word14, filename14;
    filename14 = "encoded_image123.txt";
    file14.open("encoded_image123.txt",ios::in);

     int gd = DETECT, gm,pm;
    initgraph(&gd, &gm, "C:\\TC\\BGI");

        for(int j=0;j<height;j++)
        {
        for(int i=0;i<width;i++)
        {
        file14 >> word14;
        int x=map1[word14];
        int fg=x&0x0000ff;
        for(int m=0;m<10;m++)
        {
        for(int n=0;n<10;n++)
        {
        putpixel(100+temp, 100+var, fg);
        temp++;
        }
        temp=temp1;
        var++;
        }
        var=var1;
        temp1=temp1+10;
        temp=temp+10;

        }
        var1=var1+10;
        var=var+10;
        temp=0;
        temp1=0;
        }




    file14.close();
    getch();
   closegraph();
   return 0;

}
