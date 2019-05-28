#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#define CLUB 1
#define DIAMOND 2
#define HEART 3
#define SPADE 4

#define ACE 1
#define JACK 11
#define QUEEN 12
#define KING 13

typedef struct{ int suit, rank; } card;
typedef struct Deck{ card *c; struct Deck *next; } deck;
typedef struct { deck *hand; int min, max; int count; } player;


deck* add_card(deck *d, card *c);
deck* new_deck(void);
deck *mix(deck *d);
deck* cut(deck* d);
void free_deck(deck *d);


void print_card(card *c);
void print_deck(deck *d); 
void update_player(player *p);
deck* hit(player *p, deck *d); 


deck* add_card(deck *d, card *c)
{
  deck *d_new;
  d_new = (deck *)malloc(sizeof(deck));
  assert(d_new!=NULL);
  d_new->c = c;
  d_new->next=d;
  
  return d_new;
}

deck* new_deck(void)
{
  int i=0,j=0;
  deck *d=NULL;
  card *c;
  
  for(j=CLUB;j<=SPADE;j++)
    {
      for(i=1;i<14;i++)
        {
          c = (card *)malloc(sizeof(card));
          assert(c!=NULL);
          c->suit = j;
          c->rank = i;
          d=add_card(d,c);
        }
    }
  
  return d;
}

deck *mix(deck *d)
{
  deck *d1,*d2,*tmp;
  int i;
  
  assert(d!=NULL);
  d1=d;
  
  for(i=0;i<25;i++)
    {
      d = d->next;
      assert(d!=NULL);
    }

  d2 = d->next;
  d->next = NULL;

  d=NULL;
  while(d1!=NULL)
    {
      tmp = d1->next;
      d1->next = d;
      d = d1;
      d1 = tmp;

      if(d2==NULL) break;
      tmp = d2->next;
      d2->next = d;
      d = d2;
      d2 = tmp;      
    }
  assert(d2==NULL);
  assert(d1==NULL);

  return d;
}

deck* cut(deck* d)
{
  int r = rand() % 52;
  int i = 0;
  deck *d1,*d2;
  
  d1=d;
  for(i=1;i<r;i++)
    d=d->next;
  
  d2 = d->next;
  d->next = NULL;
  
  d = d2;
  if(d2==NULL) d = d1;
  else
    {
      while(d2->next !=NULL)
        d2 = d2->next; 
      d2->next = d1;
    }
  
  return d;
}

deck *shuffle(deck *d)
{
  int i=0;
  
  for(i=0;i<10;i++)
    {
      d = cut(d);
      d = mix(d);
    }

  return d;
}

void free_deck(deck *d)
{
  deck *tmp;
  while(d!=NULL)
    {
      free(d->c);
      tmp = d->next;
      free(d);
      d = tmp;
    }
}

void print_card(card *c)
{
  assert(c!=NULL);

  if(c->rank>ACE && c->rank<JACK) printf("%2d",c->rank);
  else
    {
      switch(c->rank)
        {
        case ACE: printf(" A"); break;
        case JACK: printf(" J"); break;
        case QUEEN: printf(" Q"); break;
        case KING: printf(" K"); break;
        default: assert(0);
        }
    }
  switch(c->suit)
    {
    case CLUB: printf("c"); break;
    case DIAMOND: printf("d"); break;
    case HEART: printf("h"); break;
    case SPADE: printf("s"); break;
    default: assert(0);
    }
}

void print_deck(deck *d)
{
  assert(d!=NULL);
  
  /* Printa a pilha do último para o primeiro */
  while(d!=NULL)
    {
      print_card(d->c);
      printf(" ");
      d = d->next;
    }
}

void update_player(player *p)
{
  int aces=0, i;
  int total=0;
  deck *d;
  
  assert(p!=NULL);

  if(p->hand==NULL) 
    {
      p->min = 0;
      p->max = 0;
      return;
    }

  d = p->hand;
  while(d!=NULL)
    {
      switch(d->c->rank) 
        {
        case ACE: aces++; break;
        case KING: 
        case QUEEN:
        case JACK: total+=10; break;
        default: total+=d->c->rank;
        }

      d=d->next;
    }
  
  p->min = aces+total;
  p->max = p->min;

  i=aces;
  while(p->max <= 21 && i>0)
    {
      p->max +=10;
      i--;
    }
  
  if(p->max > 21 && i!=aces) p->max-=10;
  
}

deck* hit(player *p, deck *d)
{
  deck *tmp;

  assert(d!=NULL);
  assert(p!=NULL);

  tmp = d->next;
  d->next = p->hand;
  p->hand = d;

  p->count = p->count+1;
  
  update_player(p);

  return tmp;
}

int main(void)
{
  deck *d;
  player p, dealer;
  char input[80];

  printf("Jogo Blackjack \n");
  printf("-------------- \n");
  printf("Jogador vs Dealer \n");
  
  srand(521);
  
  do 
    {
      /* Inicializa os jogadores */
      p.hand=NULL; 
      p.min=0;
      p.max=0;
      p.count=0;
      dealer.hand=NULL;
      dealer.min=0;
      dealer.max=0;
	  dealer.count=0;
	  
      printf("Embaralhando as cartas....\n");
      d=new_deck();
      d=shuffle(d);
      
      
      d=hit(&p,d);
      d=hit(&dealer,d);
      d=hit(&p,d);
      d=hit(&dealer,d);

      printf("Dealer: ");
      print_deck(dealer.hand);
      printf("\nJogador: ");
      print_deck(p.hand);
      
      printf("\nTotal do Jogador %d", p.max);
      input[0]='\0';
      while(input[0]!='s')
        {
          printf("\n(m)Mais ou (s)Segurar\n");
          scanf("%s",input);
          
          input[0]=tolower(input[0]);
          
          if(input[0]=='m')
            {
              d = hit(&p,d);
              printf("você: ");
              print_deck(p.hand);
              printf("\n", p.min);
              printf("Total de cartas do Dealer : %d\n",dealer.count);
  			  printf("Total de suas cartas: %d\n", p.count);
  			  printf("\nTotal do Jogador %d", p.max);
              if(p.min>21) break;
            }
        }
      
      if(p.min>21) {
      	printf("\nVocê perdeu! Dealer ganhou!!!\n");
        printf("\nSeus pontos: %d vs Dealer: %d\n", p.max, dealer.max);
  		printf("\nCartas pedidas do Dealer : %d, cartas pedidas: %d\n", dealer.count, p.count);
	  }
      else
        {
          
          while(dealer.max<17)
            {
              d = hit(&dealer,d);
              printf("\nDealer: ");
              print_deck(dealer.hand);
              if(dealer.min>21) break;
            }
          
          if(dealer.min>21) {
              printf("\nMerda. Eu perdi (Jogador ganhou!).\n");
        	  printf("\nSeus pontos: %d vs Dealer: %d\n", p.max, dealer.max);
          } else {
              if(dealer.max>p.max) {
                printf("\nEu venci (Jogador perdeu!).\n");
                printf("\nTotal do Jogador: %d vs Total Dealer: %d\n", p.max, dealer.max);
			  }
              else{
                printf("\nJogador venceu.\n");
       			printf("\nSeus pontos: %d vs Dealer: %d\n", p.max, dealer.max);
			  }
            }
  	  		printf("\nCartas pedidas do Dealer : %d, cartas pedidas: %d\n", dealer.count, p.count);
        }
      
      free_deck(p.hand);
      free_deck(dealer.hand);
      free_deck(d);

      printf("\nJogar novamente? ");
      scanf("%s",input);
      input[0]=tolower(input[0]);
      
    } while(input[0]!='n');
  return 0;
}

