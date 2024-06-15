// Yunus Emre Ay / 21100011016 / TR.yunus.emre.ay@gmail.com / 05333244768


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define max_size 7

struct fonksiyon_parametreleri // thread fonksiyonuna gönderilecek parametreler struct yapisinda tutulmaktadir.
{
    int dizi1[max_size][max_size];
    int max_stun;
    int dizi2[max_size][max_size];
    int eleman_satir;
    int eleman_stun;
};

pthread_t threadss[max_size][max_size]; // carpma islemini gerceklestirecek threadler.
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int DiziSon[max_size][max_size]={0}; // Carpim sonucu burada tutulmaktadir.
int tempSum;

int stun_sayisi(FILE *dosya) // Dosyalarin stun sayisini belirlemektedir.
{
    fseek(dosya,0,SEEK_SET); // Dosya isaretcisi en basa goturuldu.

    int sayac = 0;
    char karakter;

    while(!feof(dosya))
    {
        karakter = fgetc(dosya);
        if (karakter == '\n')
        {
            return sayac;
        }
        else if (karakter == ' ')
        {
            sayac++;
        }
    }
    return 0;
}

int satir_sayisi(FILE *dosya) // Dosyalarin satir sayisini belirlemektedir.
{
    fseek(dosya,0,SEEK_SET); // Dosya isaretcisi en basa goturuldu.

    int sayac = 0;
    char karakter, sonraki;

    while(!feof(dosya))
    {
        karakter = fgetc(dosya);
        if (karakter == '\n')
        {
            sayac++;
            sonraki=fgetc(dosya);
        }
    }

    if(sonraki!=EOF)
    {
    	sayac++;
    }
    return sayac;
}


// islem yapilacak dosya ismi - islem yapilacak dizi ismi - kullanilacak stun sayisi bilgilerini alarak, dosya icerisinde bulunan dizi programa cekilir.
void Dizi_doldur(FILE *dosya,int dizi[][max_size],int stun_Sayisi)
{
    fseek(dosya,0,SEEK_SET); // Dosya isaretcisi en basa goturuldu.
    int i=0, j=0;
    while(!feof(dosya))
    {
        fscanf(dosya,"%d",&dizi[i][j]);
        j++;
        if (stun_Sayisi<=j)
        {
            i++;
            j=0;
        }
    }
}

// ilgili parametreler struct yapisi kullanilarak fonksiyon icerisine cekilir, carpma islemleri yapilir ve sonuc "DiziSon" icerisine yazilir.
void *matris_carpma(void *B)
{
    pthread_mutex_lock(&mutex);

    struct fonksiyon_parametreleri *A = (struct fonksiyon_parametreleri *) B;

    int i;
    tempSum = 0;
    for(i=0;i<A->max_stun;i++)
    {
        tempSum += (A->dizi1[A->eleman_satir][i])*(A->dizi2[i][A->eleman_stun]);
    }
    DiziSon[A->eleman_satir][A->eleman_stun] = tempSum;

    pthread_mutex_unlock(&mutex);

    return DiziSon;
}

void yazdir(int satir_sayisi,int stun_sayisi) // "DiziSon" ekrana yazdirilir.
{
    int i=0,j=0;
    for(i=0;i<satir_sayisi;i++)
    {
        for(j=0;j<stun_sayisi;j++)
        {
            printf("%d ",DiziSon[i][j]);
        }
        printf("\n");
    }
}


int main()
{

    FILE *inputA = fopen("inputA.txt","r");
    FILE *inputB = fopen("inputB.txt","r");

    if (inputA == NULL)
    {
        printf("inputA dosyasi acilamadi.");
        return 1;
    }
    else if (inputB == NULL)
    {
        printf("inputB dosyasi acilamadi.");
        return 1;
    }

    int satirA,stunA,satirB,stunB; // satir - stun sayisi tutulmaktadir.
    int DiziA[max_size][max_size]={0},DiziB[max_size][max_size]={0}; // Dosyada bulunan dizi elemanlari tutulmaktadir.
    int i=0,j=0;

    satirA = satir_sayisi(inputA);
    satirB = satir_sayisi(inputB);

    stunA = stun_sayisi(inputA);
    stunB = stun_sayisi(inputB);

    Dizi_doldur(inputA,DiziA,stunA);
    Dizi_doldur(inputB,DiziB,stunB);

    // "matris_carpma" fonksiyonuna verilecek parametre degerlerini icermektedir.
    struct fonksiyon_parametreleri *A = (struct fonksiyon_parametreleri*)malloc(sizeof(struct fonksiyon_parametreleri));

    if (stunA == satirB) // "stun sayisi" = "satir sayisi" olacak sekilde fonksiyonlar konumlandirilmaktadir.
    {
        A->max_stun=stunA;

        for (i = 0; i < max_size; i++) // dosyadan okunan diziler struct yapisina aktarilmaktadir.
        {
            for (j = 0; j < max_size; j++)
            {
                A->dizi1[i][j] = DiziA[i][j];
                A->dizi2[i][j] = DiziB[i][j];
            }
        }

        for(i=0;i<satirA;i++)
        {
            for(j=0;j<stunB;j++)
            {
                A->eleman_satir=i;
                A->eleman_stun=j;

                pthread_create(&threadss[i][j], NULL, matris_carpma, A); // Her carpma islemi, thread dizisi kullanilarak farkli bir thread ile yapilmaktadir.
                pthread_join(threadss[i][j], NULL);
            }
        }

        yazdir(satirA,stunB);
    }
    else if (stunB == satirA) // "stun sayisi" = "satir sayisi" olacak sekilde fonksiyonlar konumlandirilmaktadir.
    {
        A->max_stun=stunB;

        for (i = 0; i < max_size; i++) // dosyadan okunan diziler struct yapisina aktarilmaktadir.
        {
            for (j = 0; j < max_size; j++)
            {
                A->dizi1[i][j] = DiziB[i][j];
                A->dizi2[i][j] = DiziA[i][j];
            }
        }

        for(i=0;i<satirB;i++)
        {
            for(j=0;j<stunA;j++)
            {
                A->eleman_satir=i;
                A->eleman_stun=j;

                pthread_create(&threadss[i][j], NULL, matris_carpma, A); // Her carpma islemi, thread dizisi kullanilarak farkli bir thread ile yapilmaktadir.
                pthread_join(threadss[i][j], NULL);
            }
        }
        yazdir(satirB,stunA);
    }
    else
    {
        printf("A ve B matrisleri uygun satir - stun degerlerini saglamadigi icin carpilamamaktadir.");
        return 1;
    }

    fclose(inputA);
    fclose(inputB);
    free(A);

    return 0;
}
