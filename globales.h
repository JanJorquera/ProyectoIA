#ifndef externo
int Tinstancia;
#endif

#ifdef externo
extern int Tinstancia;
#endif

struct coordenada{
  float x, y;
};



  
class individuo {
  friend ostream &operator<<(ostream &, const individuo &);

public:
  int aptitud;
  bool usado;
  vector <string> cromosoma;

  individuo();
  individuo(const individuo &);
  ~individuo(){};
  individuo &operator=(const individuo &rhs);
  int operator==(const individuo &rhs) const;
  int operator<(const individuo &rhs) const;
};

individuo::individuo() { // Constructor
    aptitud = AMAX;
    usado = true;
    cromosoma=vector<string>(Tinstancia, "1");
    /*for(int i=0; i<Tinstancia; i++)
    {
      cromosoma.push_back(1);
    }*/
}
  
individuo::individuo(const individuo &a) {
    aptitud = a.aptitud;
    usado = a.usado;
    cromosoma = a.cromosoma;
}

ostream &operator<<(ostream &output, const individuo &a) {
    output << "[" << &a << "]";
    output << "[" << a.aptitud << "]";  
    output << "[" << a.usado << "]";
    for(int i=0; i<a.cromosoma.size(); i++)
      output << a.cromosoma[i] <<" ";
    //output<<endl;
    return output;
}

individuo& individuo::operator=(const individuo &a) {
    this->aptitud = a.aptitud;
    this->usado = a.usado;
    this->cromosoma = a.cromosoma;

    return *this;
}

int individuo::operator==(const individuo &a) const {
    //Ojo!, dos anticuerpos son iguales si tienen la misma aptitud
    if( this->aptitud != a.aptitud) return 0;
    return 1;
}

int individuo::operator<(const individuo &a) const {
    if( this->aptitud < a.aptitud ) return 1;
    return 0;
}







class conjunto {
  friend ostream &operator<<(ostream &, conjunto &);

 public:
  char name[TAM];
  vector <individuo> conj;
  conjunto(char n[TAM]);   // Constructor
  ~conjunto();
  void vaciar();
};

conjunto::conjunto(char n[TAM]){   // Constructor 
    strncpy(name, n, TAM);
  }

 conjunto::~conjunto(void) {
    int size = conj.size();
    for(int i=0; i<size; i++)
    {
      conj[i].cromosoma.clear();
    }
    conj.clear();
  }


void conjunto::vaciar(void) {
    int size = conj.size();
    for(int i=0; i<size; i++)
    {
      conj[i].cromosoma.clear();
    }
    conj.clear();
  }

ostream &operator<<(ostream &output, conjunto & c ) {
  vector<individuo>::iterator i;
  output<<c.name<<endl;
  output<<c.conj.size()<<endl;
  if(c.conj.size() > 0) {
      for(i = (c.conj.begin()); i != (c.conj.end()); ++i)
	      output << *i << endl; // print all
  }
  output << endl;
  return output;
}


#ifndef externo
time_t Inicio, Fin_opt;
string archivo_configuracion;
string archivo_resultados;

int **distancia;
coordenada *coordenadas;
int tipodistancia;

individuo optimo_global;
individuo optimo;
int semilla;

int ps;
float mr, cr;
int so, eo, kTournament;
/* int so, co, mo, eo; */
int mutaciones, cruzamientos;

int iteracion, max_iter, evaluaciones, maximo_evaluaciones;
float aptitud_opt;
int iteracion_opt;
int evaluacion_opt;
ofstream resultados;
bool optimo_encontrado;

string archivo_convergencia = "convergencia.txt";
#endif

/*
int int_rand (int , int );
float float_rand (float , float );

void calcular_aptitud(individuo * );

void validar_permutacion(individuo * );
void escribir_en_archivo_resultados(individuo *);
*/
void calcular_aptitud(individuo * );
void readConfiguration(void);
void salir(void);
/*
float absoluto( float );
float promedio(float , float );
*/
