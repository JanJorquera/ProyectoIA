#include "includes.h"
#include "globales.h"

bool debug = false;

// Parámetros del problema
int N, HP1, D;
vector<double> T;
vector<double> S;
vector<vector<double>> t;
vector<vector<double>> coord;

static void control_c (int signo) {
  //escribir_en_archivo_resultados(&optimo);
  exit (1);
}

//Leer inputs por consola
int Leer_entradas(int argc, char **argv) {
  //archivo con la instancia del problema
  archivo_configuracion = argv[1];
  //archivo donde escribir los resultados de la ejecucion
  archivo_resultados = argv[2];
  
  //PARAMETROS
  eo = atoi(argv[3]); //elitism operator
  cr = atof(argv[4]); //crossover rate
  mr = atof(argv[5]); //mutation rate
  ps = atoi(argv[6]); //population size
  if(debug) {
    printf("eo: %d, mr: %.2f, cr: %.2f, ps: %d\n", eo, mr, cr, ps);
    //getchar();
  }
  //BUDGET
  max_iter = (int)(atof(argv[7]));
  maximo_evaluaciones = max_iter;
  //SEMILLA
  semilla = atoi (argv[8]);
  return 1;
}

void salir(void) {
  // escribir_en_archivo_resultados(&optimo);
  //salida sin error
  exit(0);
}



// Leer archivo de instancias
void readConfiguration(string fileName, int &N, int &HP1, int &D, vector<double> T, vector<double> S, vector<vector<double>> coord, vector<vector<double>> t) {
  ifstream file(fileName);
  if (!file) {
    cerr << "Error al abrir el archivo" << endl;
    exit(1);
  }

  string line;

  int count = 1;
  while (getline(file, line)) {
    if (line.length() != 0) {
      istringstream lineStream(line);
      double value;

      try {
        if (count == 1) {
          lineStream >> N >> HP1 >> D;
          N = N - 2;
          HP1 = HP1 + 2;
          coord.resize(N + HP1, vector<double>(2));
          t.resize(N + HP1, vector<double>(N + HP1));
        } else if (count == 2) {
          // Pendiente
          cout << "Pendiente" << endl;
        } else if (count == 3) {
          while (lineStream >> value) {
            T.push_back(value);
          }
        } else if (count >= 5 && count <= N + HP1 + 4) {
          double X, Y, s;
          lineStream >> X >> Y >> s;
          coord[count - 5].push_back(X);
          coord[count - 5].push_back(Y);
          cout << "x: " << coord[count - 5][0] << "y: " << coord[count - 5][1] << endl;
          S.push_back(s);
        }
      } catch (const exception& e) {
        cerr << "Error en formato de archivo" << endl;
        exit(1);
      }
      
    } else {
      if (count != 4) {
        cerr << "Error en formato de archivo" << endl;
        exit(1);
      }
    }
    count++;
  }

  file.close();
  return;
}


//Generador numero aleatorio entero
int int_rand (int a, int b) {
  //generacion de numeros aleatorios entre a y b-1
  int retorno = 0;
  if (a < b) { 
    //retorno cuando a < b
    retorno = (int) ((b - a) * drand48 ());
    retorno = retorno + a;
  } else {
    //retorno cuando b <= a
    retorno = (int) ((a - b) * drand48 ());
    retorno = retorno + b;
  }
  return retorno;
}

//Generador numero aleatorio float
float float_rand (float a, float b) {
  //generacion de numeros aleatorios entre a y b-1
  float retorno = 0.00;
  if (a < b) {
    //retorno cuando a < b
    retorno = ((b - a) * drand48 ());
    retorno = retorno + a;
  } else {
    //retorno cuando b <= a
    retorno = ((a - b) * drand48 ());
    retorno = retorno + b;
  }
  return retorno;
}


void calculateDistances(vector<vector<double>> coord, vector<vector<double>> t) {
  cout << "tam: " << coord.size() << endl;
  for (size_t i = 0; i < coord.size(); ++i) {
    for (size_t j = 0; j < coord.size(); ++j) {
      t[i][j] = sqrt(pow(coord[i][0] - coord[j][0], 2.0) + pow(coord[i][1] - coord[j][1], 2.0));
    }
  }
  for (size_t i = 0; i < coord.size(); ++i) {
    for (size_t j = 0; j < coord.size(); ++j) {
      printf("t[%d][%d] = %.5f\n",i,j,t[i][j]);
    }
  }
}


int getPos(string vertex){
  if (vertex.find("H") == 0) {
    return stoi(vertex.substr(1));
  }
  return HP1+stoi(vertex);
}

bool checkTripFeasibility(vector<string> Trip, int numTrip){
  int costo = 0;
  int posInicio = getPos(Trip[0]);
  int posFin;

  for (int i=1; i<Trip.size(); i++){
    posFin = getPos(Trip[i]);
    cout << "posInicio: " << posInicio << endl;
    cout << "posFin: " << posFin << endl;
    costo += t[posInicio][posFin];
    cout << "costo: " << costo << endl;
    if (T[numTrip] > costo){
      return false;
    }
    posInicio = posFin;
    cout << "i: " << i << endl;
  }
  return true;
}

void guardar_optimo_encontrado (int aptitud_optimo, individuo temp) {
  //recuerda el optimo encontrado
  optimo_global = temp;
  iteracion_opt = iteracion;
  evaluacion_opt = evaluaciones;
  optimo_encontrado = true;
  //muestra el individuo encontrado como optimo
  if(debug)
  {
      cout<<endl<<"Aptitud: "<<aptitud_optimo;
  }
  salir();
  return;
}


void calcular_aptitud(individuo * temp) {
  //Reevaluar solo los individuos con cambios
  if(temp->usado==false) {
    return;  
  }
  float apt=0;
  //calcular el costo del tour
  for(int i=0; i<(Tinstancia-1); i++){
    apt += t[getPos(temp->cromosoma[i])][getPos(temp->cromosoma[i+1])];
  }
  
  temp->aptitud = apt;

  if (optimo_global.aptitud == AMAX || apt >= optimo_global.aptitud) {
    guardar_optimo_encontrado(apt, *temp);
  }

  //conteo de la cantidad de evaluaciones realizadas en el algoritmo
  evaluaciones++;
  //chequeo de maximo de evaluaciones
  if(evaluaciones >= maximo_evaluaciones){
    salir();
  }
  return;
}

void agregar_individuo_aleatorio (conjunto & c_temp) {
  int rand;
  individuo i_temp;
  i_temp.usado = true;

  vector<string> set;
  vector<string> POIs;
  vector<string> Trip;

  cout << "tinstancia: " << Tinstancia << endl;
  for(int i=0; i< Tinstancia; i++) {
    if (i<HP1){
      set.push_back("H"+to_string(i));
    } else {
      set.push_back(to_string(i-HP1));
    }
  }

  for (int i=0; i<set.size(); i++){
    cout << "Pos i de set: " << set[i] << endl;
  }

  
  string Hinicio = "H0";
  string Htermino;
  set.erase(set.begin() + 0);
  i_temp.cromosoma.push_back(Hinicio);
  int cantHotelesQuitados = 1;
  
  
  bool agregarPOI;
  for (int trip=0; trip<D; trip++){
    Trip.push_back(Hinicio);
    if (trip==D-1){
      Htermino = "H1";
    } else {
      Htermino = "H"+to_string(int_rand(1, (HP1 - cantHotelesQuitados)));
    }
    Trip.push_back(Htermino);
    set.erase(set.begin() + getPos(Htermino));
    cantHotelesQuitados++;

    agregarPOI = true;
    int POIpos;
    string POI;
    while (agregarPOI) {
      if (set.size() == 0){
        break;
      }
      POIpos = int_rand(HP1 - cantHotelesQuitados, set.size());
      cout << "POIpos: " << POIpos << endl;
      POI = set[POIpos];
      cout << "set[POIpos]: " << set[POIpos] << endl;
      set.erase(set.begin() + getPos(POI));
      cout << "check" << endl;
      // POIs.erase(set.begin() + stoi(POI));
      Trip.insert(Trip.end()-1,POI);
      cout << "check1" << endl;
      if (checkTripFeasibility(Trip, trip)){
        i_temp.cromosoma.push_back(POI);
      } else {
        agregarPOI = false;
      }
      cout << "check2" << endl;
    }

    i_temp.cromosoma.push_back(Htermino);
    Hinicio = Htermino;
    Trip.clear();
  }

  cout << "fin" << endl;


  //calculo de la aptitud del individuo
  calcular_aptitud(&i_temp);
  c_temp.conj.push_back(i_temp);
  return;
}


void crear_poblacion_inicial(conjunto &poblacion, int size){
  cout << "size: " << size << endl;
  for (int i=0; i<size; i++) {
    agregar_individuo_aleatorio(poblacion);
  }
  return;
}




int main(int argc, char *argv[]) {
  //lectura de parametros
  if(!Leer_entradas(argc,argv)){
    cout<<"Problemas en la lectura de los parametros";
    exit(1);
  }

  struct timeval hora;
  if (signal (SIGINT, control_c) == SIG_ERR)
    printf ("can't catch SIGTERM");
  
  

  //lectura de instancia
  iteracion = -1;
  readConfiguration(archivo_configuracion, N, HP1, D, T, S, coord, t);
  Tinstancia = (D+1)+N;
  calculateDistances(coord, t);
  optimo_encontrado = false;
  evaluaciones = 0;

  //medicion de tiempo
  Inicio=time(NULL);

  //semilla aleatoria
  srand48 (semilla);

  //creacion de poblacion inicial aleatoria
  // inicializar_archivo_convergencia();
  cout << "N: " << N << " HP1: " << HP1 << " D: " << D << endl;
  conjunto poblacion ((char*)"poblacion");
  crear_poblacion_inicial(poblacion, ps);

  if(debug) {
    cout<<poblacion;
    getchar();
  }

  //contador de mutaciones
  mutaciones=0;
  //int evaluaciones_ant = 0;
  //int prom_it;
  for (iteracion = 0; iteracion < max_iter; iteracion++) {
    time_t ini_it=time(NULL);
    conjunto seleccionados ((char*)"seleccionados");
    conjunto cruzados ((char*)"cruzados");
    conjunto mutados ((char*)"mutados");

    if(debug){
      cout<<poblacion;
      getchar();
    }

    /*
    seleccionar_conjunto(poblacion, seleccionados, ps);
    if(debug){
      cout<<seleccionados;
      getchar();
    }

    cruzar_conjunto(seleccionados, cruzados, cr);
    if(debug) {
      cout<<"POBLACION CRUZADA"<<endl;
      cout<<cruzados;
      getchar();
    }

    mutar_conjunto(cruzados, mutados, mr);
    if(debug){
      cout<<"POBLACION MUTADA"<<endl;
      cout<<mutados;
      getchar();
    }

    if(eo==1){
      mutados.conj.front() = *&(poblacion.conj.front());
    }

    poblacion.vaciar();
    poblacion = mutados;
    strcpy(poblacion.name, "poblacion");
    seleccionados.vaciar();
    cruzados.vaciar();
    mutados.vaciar();
    guardar_optimo(poblacion);
    */
  }
  
  salir();

  return 0;
}