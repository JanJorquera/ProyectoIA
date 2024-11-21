#include "includes.h"
#include "globales.h"

bool debug = true;
bool debugGenerateFeasibleSequences = false;

// Parámetros del problema
int N, HP1, D;
vector<double> T;
vector<double> S;
vector<vector<double>> t;
vector<vector<double>> coord;
double Tmax;

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
  so = atoi(argv[3]); //selection operator 
  eo = atoi(argv[4]); //elitism operator
  cr = atof(argv[5]); //crossover rate
  mr = atof(argv[6]); //mutation rate
  ps = atoi(argv[7]); //population size
  if(debug) {
    printf("so: %d, eo: %d, mr: %.2f, cr: %.2f, ps: %d\n", so, eo, mr, cr, ps);
    getchar();
  }
  //BUDGET
  max_iter = (int)(atof(argv[8]));
  maximo_evaluaciones = max_iter;
  //SEMILLA
  semilla = atoi (argv[9]);
  //KTOURNAMENT
  if (so == 2){
    try {
      kTournament = atoi (argv[10]);
    } catch (const exception& e) {
      cerr << "Error en tamaño de torneo" << endl;
      exit(1);
    }
  }
  return 1;
}

int getPos(string vertex){
  if (vertex.find("H") == 0) {
    return stoi(vertex.substr(1));
  }
  return HP1+stoi(vertex);
}

double calculateTripCost(vector<string> trip){
  double costo = 0;
  for (int i=0; i<trip.size()-1; i++){
    costo += t[getPos(trip[i])][getPos(trip[i+1])];
  }
  return costo;
}

double findTrip(int numTrip, individuo &individuoPoblacion){
  int numHotelesVistos = -1;
  vector<string> trip;
  bool flagAddtoTrip = false;
  for (int i=0; i<individuoPoblacion.cromosoma.size(); i++){
    if (individuoPoblacion.cromosoma[i].find("H") == 0){
      if (flagAddtoTrip){
        trip.push_back(individuoPoblacion.cromosoma[i]);
        break;
      }
      numHotelesVistos++;
      if (numHotelesVistos == numTrip){
        flagAddtoTrip = true;
      }
    }
    if (flagAddtoTrip){
      trip.push_back(individuoPoblacion.cromosoma[i]);
    }
  }

  return calculateTripCost(trip);
}

void salir(void) {
  // escribir_en_archivo_resultados(&optimo);
  //salida sin error
  ostringstream resultado;
  for (size_t i = 0; i < optimo.cromosoma.size(); ++i) {
    resultado << optimo.cromosoma[i];
    // Agrega el separador después de cada elemento, excepto el último
    if (i != optimo.cromosoma.size() - 1) {
      resultado << " - ";
    }
  }
  
  cout << resultado.str() << endl;
  cout << optimo.aptitud << endl;

  ostringstream resultadoTrip;
  double costoTrip;
  for (int i=0; i<D; i++){
    costoTrip = findTrip(i, optimo);
    resultadoTrip << "Trip " << i+1 << ": " << costoTrip;
    if (i != D - 1) {
      resultadoTrip << " - ";
    }
  }

  cout << resultadoTrip.str() << endl;

  exit(0);
}



// Leer archivo de instancias
void readConfiguration(string fileName) {
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
          lineStream >> Tmax;
        } else if (count == 3) {
          while (lineStream >> value) {
            T.push_back(value);
          }
        } else if (count >= 5 && count <= N + HP1 + 4) {
          double X, Y, s;
          lineStream >> X >> Y >> s;
          coord[count - 5][0] = X;
          coord[count - 5][1] = Y;
          // cout << "xOriginal: " << X << "yOriginal: " << Y << endl;
          // cout << "x: " << coord[count - 5][0] << "y: " << coord[count - 5][1] << endl;
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

  if (debug) {
    for (int i=0; i<T.size(); i++){
      cout << "T[i]: " << T[i] << endl;
    }

    for (int i=0; i<S.size(); i++){
      cout << "S[i]: " << S[i] << endl;
    }
    getchar();
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


void calculateDistances() {
  for (size_t i = 0; i < coord.size(); ++i) {
    for (size_t j = 0; j < coord.size(); ++j) {
      t[i][j] = sqrt(pow(coord[i][0] - coord[j][0], 2.0) + pow(coord[i][1] - coord[j][1], 2.0));
    }
  }

  if (debug) {
    cout << "tam: " << coord.size() << endl;
    for (size_t i = 0; i < coord.size(); ++i) {
      for (size_t j = 0; j < coord.size(); ++j) {
        cout << "t[" << i << "][" << j << "] = " << t[i][j] << endl;
      }
    }
    getchar();
  }
}

bool checkTripFeasibility(vector<string> Trip, int numTrip){
  double costo = 0;
  int posInicio = getPos(Trip[0]);
  int posFin;

  for (int i=1; i<Trip.size(); i++){
    posFin = getPos(Trip[i]);
    // cout << "posInicio: " << posInicio << endl;
    // cout << "posFin: " << posFin << endl;
    costo += t[posInicio][posFin];
    // cout << "costo: " << costo << endl;
    if (T[numTrip] < costo){
      return false;
    }
    posInicio = posFin;
    // cout << "i: " << i << endl;
  }
  return true;
}

void guardar_optimo_encontrado (int aptitud_optimo, individuo temp) {
  //recuerda el optimo encontrado
  optimo = temp;
  iteracion_opt = iteracion;
  evaluacion_opt = evaluaciones;
  optimo_encontrado = true;
  //muestra el individuo encontrado como optimo
  if(debug)
  {
      cout<<endl<<"Aptitud: "<<aptitud_optimo;
      getchar();
  }
  return;
}

void guardar_optimo(conjunto & c_temp) {
  sort(c_temp.conj.begin(), c_temp.conj.end());
  if(c_temp.conj.back().aptitud > optimo.aptitud){
    optimo = c_temp.conj.front();
    iteracion_opt = iteracion;
    evaluacion_opt = evaluaciones;
    Fin_opt = time(NULL);
    //cout<<iteracion<<" "<< optimo.aptitud<<endl;
  }
  //Si se cumplen XX evaluaciones sin cambio se acaba
  if((evaluaciones - evaluacion_opt)>1000000){
  	salir();
  }
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
    if (temp->cromosoma[i] == "") {
      break;
    }
    apt += S[getPos(temp->cromosoma[i])];
  }
  
  temp->aptitud = apt;

  if (optimo.aptitud == AMAX || apt > optimo.aptitud) {
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

//Seleccion de individuos//
individuo * roulette_wheel (conjunto & c_temp) {
  int i = 0;
  float rand;
  float suma = 0.00;
  individuo * i_temp;

  for (vector<individuo>::iterator p = c_temp.conj.begin (); p != c_temp.conj.end (); p++){
    suma += p->aptitud;
    i++;
  }

  //numero aleatorio para la ruleta
  rand = float_rand(0.00, 1.00);
  vector<float> rangeProbabilities;
  i=0;

  rangeProbabilities.push_back(0);
  float limInferior = 0.0;
  for (vector<individuo>::iterator p = c_temp.conj.begin (); p != c_temp.conj.end (); p++){
    rangeProbabilities.push_back(limInferior + (p->aptitud/suma));
    limInferior = limInferior + (p->aptitud/suma);
  }

  //busqueda del individuo en la ruleta
  i = 0;
  for (vector<individuo>::iterator p = c_temp.conj.begin (); p != c_temp.conj.end (); p++){
    if (rangeProbabilities[i] <= rand && rand <= rangeProbabilities[i+1]){
      i_temp = &(*p);
      return i_temp;
	  }
    i++;
  }

  //en caso de ser el ultimo individuo de la poblacion
  i_temp = (&(c_temp.conj.back()));
  return i_temp;
}

individuo * ktournament (conjunto & c_temp) {
  individuo * i_temp = nullptr;
  for (int i = 0; i < kTournament; i++) {
    individuo* current = &c_temp.conj[int_rand(0, c_temp.conj.size())];
    if (!i_temp || current->aptitud > i_temp->aptitud) {
      i_temp = current;
    }
  }
  return i_temp;
}

individuo * seleccionar_individuo (conjunto & c_temp) {
  individuo * i_temp;
  switch(so) {
    case ROULETTE:
      i_temp = roulette_wheel(c_temp);
      return i_temp;
    case TOURNAMENT:
      i_temp = ktournament(c_temp);
      return i_temp;
    default:
      cout<<"ERROR: No se reconoce tipo de seleccion"<<endl;
      salir();
      return nullptr;
   }
}

void seleccionar_conjunto(conjunto & in, conjunto & out, int n) {
  individuo * p_i_temp;
  for(int i=0; i<n; i++){
    individuo i_temp;
    p_i_temp = seleccionar_individuo(in);
    i_temp = *p_i_temp;
    out.conj.push_back(i_temp);
  }
}



//Mutación
void replaceHotel(individuo *hijo){
  vector<string> listaHotelesTour;
  vector<string> listaPois;
  int pos = int_rand(1,D);

  // cout << "tamaño hijo: " << hijo->cromosoma.size() << endl; 
  for (int i=0; i<hijo->cromosoma.size(); i++){
    if (hijo->cromosoma[i].find("H") == 0){
      listaHotelesTour.push_back(hijo->cromosoma[i]);
    } else {
      listaPois.push_back(hijo->cromosoma[i]);
    }
  }

  /*
  cout << "tamaño: " << listaHotelesTour.size() << endl;
  for (int i=0; i<listaHotelesTour.size(); i++){
    cout << "listaHoteles[" << i << "]: " << listaHotelesTour[i] << endl;
  }
  */

  ostringstream aux;
  vector<string> listaHotelesD;
  vector<string> listaHotelesDAux;
  for (int i=0; i<HP1; i++){
    aux << "H" << i;
    auto it = find(listaHotelesTour.begin(), listaHotelesTour.end(), aux.str());
    if (it != listaHotelesTour.end()){
      listaHotelesD.push_back(aux.str());
      listaHotelesDAux.push_back(aux.str());
    }
    aux.clear();
  }

  int posHotel, posHoteltobeReplaced;
  bool flagHotelSwap = false;
  size_t sizeAux;
  vector<string> tripPrevHotel;
  vector<string> tripPostHotel;
  for (int i=1; i<listaHotelesTour.size()-1; i++){
    if (flagHotelSwap){
      break;
    }
    listaHotelesDAux = listaHotelesD;
    sizeAux = listaHotelesDAux.size();
    // cout << "sizeAux: " << sizeAux << endl;
    for (size_t j=0; j<sizeAux; j++){
      // cout << "j: " << j << endl;
      posHotel = int_rand(0,listaHotelesDAux.size());
      if (t[getPos(listaHotelesTour[i-1])][getPos(listaHotelesDAux[posHotel])] <= T[i-1] && t[getPos(listaHotelesDAux[posHotel])][getPos(listaHotelesTour[i+1])] <= T[i]){
        flagHotelSwap = true;
        posHoteltobeReplaced = i;
        break;
      }
      listaHotelesDAux.erase(listaHotelesDAux.begin()+posHotel);
    }
  }

  if (flagHotelSwap){
    int numHotelesVistos = -1;
    bool flagInsertPrevTrip = true;
    bool flagInsertPostTrip = true;
    
    for (int i=0; i<hijo->cromosoma.size(); i++){
      if (hijo->cromosoma[i].find("H") == 0){
        numHotelesVistos++;
        if (numHotelesVistos+1 == posHoteltobeReplaced){
          tripPrevHotel.push_back(hijo->cromosoma[i]);
          flagInsertPrevTrip = false;
        }
        if (numHotelesVistos-1 == posHoteltobeReplaced){
          tripPostHotel.push_back(hijo->cromosoma[i]);
          flagInsertPostTrip = false;
        }
      }
      if (flagInsertPrevTrip){
        tripPrevHotel.push_back(hijo->cromosoma[i]);
      }
      if (flagInsertPostTrip){
        tripPostHotel.push_back(hijo->cromosoma[i]);
      }
    }

    vector<string> listaPoisDisponible;
    for (int i=0; i<tripPrevHotel.size(); i++){
      if (tripPrevHotel[i].find("H") != 0){
        auto it = find(listaPois.begin(), listaPois.end(), tripPrevHotel[i]);
        if (it != listaPois.end()){
          listaPoisDisponible.push_back(tripPrevHotel[i]);
        }
      }
    }

    for (int i=0; i<tripPostHotel.size(); i++){
      if (tripPostHotel[i].find("H") != 0){
        auto it = find(listaPois.begin(), listaPois.end(), tripPostHotel[i]);
        if (it != listaPois.end()){
          listaPoisDisponible.push_back(tripPostHotel[i]);
        }
      }
    }

    vector<string> tripPrev;
    vector<string> tripPost;

    tripPrev.push_back(tripPrevHotel.back());
    tripPost.push_back(tripPostHotel.front());

    float numRand;
    bool isFeasibleAddPoiInBothTrips = true;
    int posPOItoAdd, posPOI;
    string POItoAdd;
    vector<string> tripAuxiliary;
    while (isFeasibleAddPoiInBothTrips){
      if (listaPoisDisponible.size() == 0){
        break;
      }

      numRand = float_rand(0.00,1.00);
      posPOItoAdd = int_rand(0,listaPoisDisponible.size());
      POItoAdd = listaPoisDisponible[posPOItoAdd];
      
      if (numRand <= 0.5){
        //Intento de insercion en el primer trip
        tripAuxiliary = tripPrevHotel;
        posPOI = int_rand(1, tripAuxiliary.size());
        tripAuxiliary.insert(tripAuxiliary.begin() + posPOI, POItoAdd);
        tripAuxiliary.push_back(listaHotelesDAux[posHotel]);
        if (checkTripFeasibility(tripAuxiliary, posHoteltobeReplaced-1)){
          tripAuxiliary.pop_back();
          tripPrevHotel = tripAuxiliary;
        } else {
          tripAuxiliary.pop_back();
          tripAuxiliary.erase(tripAuxiliary.begin()+posPOI);
          tripPrevHotel = tripAuxiliary;

          //Probar añadir en el segundo trip
          tripAuxiliary = tripPostHotel;
          posPOI = int_rand(1, tripAuxiliary.size());
          tripAuxiliary.insert(tripAuxiliary.begin() + posPOI, POItoAdd);
          tripAuxiliary.push_back(listaHotelesDAux[posHotel]);
          if (checkTripFeasibility(tripAuxiliary, posHoteltobeReplaced-1)){
            tripAuxiliary.pop_back();
            tripPostHotel = tripAuxiliary;
          } else {
            tripAuxiliary.pop_back();
            tripAuxiliary.erase(tripAuxiliary.begin()+posPOI);
            tripPostHotel = tripAuxiliary;
          }
        }
      } else {
        //Intento de insercion en el segundo trip
        tripAuxiliary = tripPostHotel;
        posPOI = int_rand(1, tripAuxiliary.size());
        tripAuxiliary.insert(tripAuxiliary.begin() + posPOI, POItoAdd);
        tripAuxiliary.push_back(listaHotelesDAux[posHotel]);
        if (checkTripFeasibility(tripAuxiliary, posHoteltobeReplaced-1)){
          tripAuxiliary.pop_back();
          tripPostHotel = tripAuxiliary;
        } else {
          tripAuxiliary.pop_back();
          tripAuxiliary.erase(tripAuxiliary.begin()+posPOI);
          tripPostHotel = tripAuxiliary;

          //Probar añadir en el primer trip
          tripAuxiliary = tripPrevHotel;
          posPOI = int_rand(1, tripAuxiliary.size());
          tripAuxiliary.insert(tripAuxiliary.begin() + posPOI, POItoAdd);
          tripAuxiliary.push_back(listaHotelesDAux[posHotel]);
          if (checkTripFeasibility(tripAuxiliary, posHoteltobeReplaced-1)){
            tripAuxiliary.pop_back();
            tripPrevHotel = tripAuxiliary;
          } else {
            tripAuxiliary.pop_back();
            tripAuxiliary.erase(tripAuxiliary.begin()+posPOI);
            tripPrevHotel = tripAuxiliary;
          }
        }
      }
      listaPoisDisponible.erase(listaPoisDisponible.begin()+posPOItoAdd);
    }
  }
  return;
}

void InsertOrDeletePOI(individuo *hijo){
  auto calculateProbability = [](double ratio) {
    return exp(-10.00 * (1 - ratio));
  };

  double Ttotal = 0;
  for (int i=0; i<D; i++){
    Ttotal += findTrip(i,*(hijo));
  }

  vector<string> listaPois;
  for (int i=0; i<hijo->cromosoma.size(); i++){
    if (hijo->cromosoma[i].find("H") != 0){
      listaPois.push_back(hijo->cromosoma[i]);
    }
  }

  //Alta probabilidad de eliminar cuando se consume casi todo el presupuesto total, la cual decrece rápidamente para fomentar inserciones y aspirar a mejoras.
  if (float_rand(0.00,1.00) <= calculateProbability(Ttotal/Tmax)){
    //Caso eliminación
    int pos = stoi(listaPois[int_rand(0,static_cast<int>(listaPois.size()))]);
    for (int i=0; i<hijo->cromosoma.size(); i++){
      if (hijo->cromosoma[i] == to_string(pos)){
        hijo->cromosoma.erase(hijo->cromosoma.begin() + i);
        mutaciones++;
        return;
      }
    }
  } else {
    //Caso inserción
    vector<string> listAuxiliaryPois;
    for (int i=0; i<N; i++){
      auto it = find(listaPois.begin(), listaPois.end(), to_string(i));
      if (it == listaPois.end()){
        //Agregar a lista auxiliar solo aquellos POIs no visitados
        listAuxiliaryPois.push_back(to_string(i));
      }
    }

    int pos = stoi(listAuxiliaryPois[int_rand(0,static_cast<int>(listAuxiliaryPois.size()))]);

    vector<int> trips;
    int posTrip;
    int cantTripsChequeados = 0;
    for (int i=0; i<D; i++){
      trips.push_back(i);
    }

    int numHotelesVistos = -1;
    vector<double> tripCost;
    for (int i=0; i<hijo->cromosoma.size(); i++){
      if (hijo->cromosoma[i].find("H") == 0){
        if (numHotelesVistos != -1){
          tripCost[numHotelesVistos]+=t[getPos(hijo->cromosoma[i-1])][getPos(hijo->cromosoma[i])];
          if (debug){
            if (t[getPos(hijo->cromosoma[i-1])][getPos(hijo->cromosoma[i])] == 0){
              cout << "Desde " << hijo->cromosoma[i-1] << " a " << hijo->cromosoma[i] << endl;
              cout << "Pues va desde: " << coord[getPos(hijo->cromosoma[i-1])][0] << "," << coord[getPos(hijo->cromosoma[i-1])][1] << " a " << coord[getPos(hijo->cromosoma[i])][0] << "," << coord[getPos(hijo->cromosoma[i])][1] << endl;
              cout << "Posicion en getPos del 1: " << getPos(hijo->cromosoma[i-1]) << " y del 2 " << getPos(hijo->cromosoma[i]) << endl;
              getchar();
            }
          }
        }
        if (i!=hijo->cromosoma.size()-1){
          numHotelesVistos++;
          tripCost.push_back(0.00);
        }
      } else {
        tripCost[numHotelesVistos]+=t[getPos(hijo->cromosoma[i-1])][getPos(hijo->cromosoma[i])];
        if (debug){
          if (t[getPos(hijo->cromosoma[i-1])][getPos(hijo->cromosoma[i])] == 0){
            cout << "Desde " << hijo->cromosoma[i-1] << " a " << hijo->cromosoma[i] << endl;
            cout << "Pues va desde: " << coord[getPos(hijo->cromosoma[i-1])][0] << "," << coord[getPos(hijo->cromosoma[i-1])][1] << " a " << coord[getPos(hijo->cromosoma[i])][0] << "," << coord[getPos(hijo->cromosoma[i])][1] << endl;
            cout << "Posicion en getPos del 1: " << getPos(hijo->cromosoma[i-1]) << " y del 2 " << getPos(hijo->cromosoma[i]) << endl;
            getchar();
          }
        }
      }
    }

    double posNewCost;
    int auxTrip;
    bool isPOIinserted = false;
    bool flagTripFound;
    for (int j=0; j<D; j++){
      if (isPOIinserted){
        break;
      }

      auxTrip = int_rand(0,D-cantTripsChequeados);
      posTrip = trips[auxTrip];
      trips.erase(trips.begin()+auxTrip);

      numHotelesVistos = -1;
      flagTripFound = false;
      for (int i=0; i<hijo->cromosoma.size(); i++){
        if (hijo->cromosoma[i].find("H") == 0){
          if (flagTripFound){
            posNewCost = tripCost[posTrip] + (t[getPos(hijo->cromosoma[i-1])][getPos(to_string(pos))] + t[getPos(to_string(pos))][getPos(hijo->cromosoma[i])]) - (t[getPos(hijo->cromosoma[i-1])][getPos(hijo->cromosoma[i])]);
            if (posNewCost <= T[posTrip]){
              hijo->cromosoma.insert(hijo->cromosoma.begin() + i, to_string(pos));
              isPOIinserted = true;
              mutaciones++;
            }
            break;
          }

          numHotelesVistos++;
          if (numHotelesVistos == posTrip){
            flagTripFound = true;
            cantTripsChequeados++;
          }
        } else {
          if (flagTripFound){
            posNewCost = tripCost[posTrip] + (t[getPos(hijo->cromosoma[i-1])][getPos(to_string(pos))] + t[getPos(to_string(pos))][getPos(hijo->cromosoma[i])]) - (t[getPos(hijo->cromosoma[i-1])][getPos(hijo->cromosoma[i])]);
            if (posNewCost <= T[posTrip]){
              hijo->cromosoma.insert(hijo->cromosoma.begin() + i, to_string(pos));
              isPOIinserted = true;
              mutaciones++;
              break;
            }
          }
        }
      }
    }

  }

  /*
  //Seleccionar un POI aleatorio
  int pos = int_rand(0,N);
  for (int i=0; i<hijo->cromosoma.size(); i++){
    //Si esta en el tour, se elimina
    if (hijo->cromosoma[i] == to_string(pos)){
      hijo->cromosoma.erase(hijo->cromosoma.begin() + i);
      return;
    }
  }

  //Si no esta, se añade en el primer trip aleatorio cuya incorporación resulte en un trip factible. Si dicho trip no existe, simplemente no se muta
  vector<int> trips;
  int posTrip;
  int cantTripsChequeados = 0;
  for (int i=0; i<D; i++){
    trips.push_back(i);
  }

  int numHotelesVistos = -1;
  vector<double> tripCost;
  for (int i=0; i<hijo->cromosoma.size(); i++){
    if (hijo->cromosoma[i].find("H") == 0){
      if (numHotelesVistos != -1){
        tripCost[numHotelesVistos]+=t[getPos(hijo->cromosoma[i-1])][getPos(hijo->cromosoma[i])];
        if (debug){
          if (t[getPos(hijo->cromosoma[i-1])][getPos(hijo->cromosoma[i])] == 0){
            cout << "Desde " << hijo->cromosoma[i-1] << " a " << hijo->cromosoma[i] << endl;
            cout << "Pues va desde: " << coord[getPos(hijo->cromosoma[i-1])][0] << "," << coord[getPos(hijo->cromosoma[i-1])][1] << " a " << coord[getPos(hijo->cromosoma[i])][0] << "," << coord[getPos(hijo->cromosoma[i])][1] << endl;
            cout << "Posicion en getPos del 1: " << getPos(hijo->cromosoma[i-1]) << " y del 2 " << getPos(hijo->cromosoma[i]) << endl;
            getchar();
          }
        }
      }
      if (i!=hijo->cromosoma.size()-1){
        numHotelesVistos++;
        tripCost.push_back(0.00);
      }
    } else {
      tripCost[numHotelesVistos]+=t[getPos(hijo->cromosoma[i-1])][getPos(hijo->cromosoma[i])];
      if (debug){
        if (t[getPos(hijo->cromosoma[i-1])][getPos(hijo->cromosoma[i])] == 0){
          cout << "Desde " << hijo->cromosoma[i-1] << " a " << hijo->cromosoma[i] << endl;
          cout << "Pues va desde: " << coord[getPos(hijo->cromosoma[i-1])][0] << "," << coord[getPos(hijo->cromosoma[i-1])][1] << " a " << coord[getPos(hijo->cromosoma[i])][0] << "," << coord[getPos(hijo->cromosoma[i])][1] << endl;
          cout << "Posicion en getPos del 1: " << getPos(hijo->cromosoma[i-1]) << " y del 2 " << getPos(hijo->cromosoma[i]) << endl;
          getchar();
        }
      }
    }
  }

  double posNewCost;
  int auxTrip;
  bool isPOIinserted = false;
  bool flagTripFound;
  for (int j=0; j<D; j++){
    if (isPOIinserted){
      break;
    }

    auxTrip = int_rand(0,D-cantTripsChequeados);
    posTrip = trips[auxTrip];
    trips.erase(trips.begin()+auxTrip);

    numHotelesVistos = -1;
    flagTripFound = false;
    for (int i=0; i<hijo->cromosoma.size(); i++){
      if (hijo->cromosoma[i].find("H") == 0){
        if (flagTripFound){
          posNewCost = tripCost[posTrip] + (t[getPos(hijo->cromosoma[i-1])][getPos(to_string(pos))] + t[getPos(to_string(pos))][getPos(hijo->cromosoma[i])]) - (t[getPos(hijo->cromosoma[i-1])][getPos(hijo->cromosoma[i])]);
          if (posNewCost <= T[posTrip]){
            hijo->cromosoma.insert(hijo->cromosoma.begin() + i, to_string(pos));
            isPOIinserted = true;
          }
          break;
        }

        numHotelesVistos++;
        if (numHotelesVistos == posTrip){
          flagTripFound = true;
          cantTripsChequeados++;
        }
      } else {
        if (flagTripFound){
          posNewCost = tripCost[posTrip] + (t[getPos(hijo->cromosoma[i-1])][getPos(to_string(pos))] + t[getPos(to_string(pos))][getPos(hijo->cromosoma[i])]) - (t[getPos(hijo->cromosoma[i-1])][getPos(hijo->cromosoma[i])]);
          if (posNewCost <= T[posTrip]){
            hijo->cromosoma.insert(hijo->cromosoma.begin() + i, to_string(pos));
            isPOIinserted = true;
            break;
          }
        }
      }
    }
  }*/
  return;
}

void mutar_individuo(individuo * padre, individuo * hijo, float mr) {
  // cout << "tamaño padre: " << padre->cromosoma.size() << endl;
  *hijo = *padre;
  /*cout << "tamaño: " << hijo->cromosoma.size() << endl;
  for (int i=0; i<hijo->cromosoma.size(); i++){
    cout << "hijo->cromosoma[" << i << "]: " << hijo->cromosoma[i] << endl;
  }*/

  /*
  replaceHotel(hijo);
  InsertOrDeletePOI(hijo);
  */

  if (float_rand(0.00,1.00) <= mr){
    replaceHotel(hijo);
    InsertOrDeletePOI(hijo);
  }
  return;
}

void mutar_conjunto(conjunto & in, conjunto & out, float mr) {
  individuo * p_seleccionado;
  for (vector<individuo>::iterator p = in.conj.begin (); p != in.conj.end (); p++){
    individuo mutado;
    p_seleccionado = &*p;
    mutar_individuo(p_seleccionado, &mutado, mr);
    calcular_aptitud(&mutado);
    out.conj.push_back(mutado);
  }
  return;
}

vector<string> getListaHoteles(individuo * individual){
  vector<string> listaHoteles;
  for (int i=0; i<individual->cromosoma){
    if (individual->cromosoma[i].find("H") == 0){
      listaHoteles.push_back(individual->cromosoma[i]);
    }
  }
  return listaHoteles;
}

bool checkRepeatedHotels(vector<string> listaHotelesP1, vector<string> listaHotelesP2){
  unordered_set<string> conjunto(listaHotelesP2.begin(), listaHotelesP2.end());

  for (const string& elemento : listaHotelesP1) {
      if (conjunto.find(elemento) != conjunto.end()) {
          std::cout << elemento << " está en ambos vectores.\n";
          return true;
      }
  }
  return false;
}

void onepointcrossover(individuo * padre1, individuo * padre2, individuo * hijo1, individuo * hijo2){
  vector<string> listaHotelesP1 = getListaHoteles(padre1);
  vector<string> listaHotelesP2 = getListaHoteles(padre2);
  vector<string> listaHotelesP1Aux1(padre1);
  vector<string> listaHotelesP2Aux1(padre2);
  vector<string> tripAuxiliar;

  listaHotelesP1Aux1.erase(listaHotelesP1Aux1.begin());
  listaHotelesP2Aux1.erase(listaHotelesP2Aux1.begin());
  listaHotelesP1Aux1.erase(listaHotelesP1Aux1.end() - 1);
  listaHotelesP2Aux1.erase(listaHotelesP2Aux1.end() - 1);

  vector<string> listaHotelesP1Aux2(listaHotelesP1Aux1);
  vector<string> listaHotelesP2Aux2(listaHotelesP2Aux1);

  if listaHotelesP1.size()<=3{
    return;
  }

  int posHP1 = -1;
  int posHP2 = -1;
  for (size_t i=1; i<listaHotelesP1.size()-2; i++){
    //Padre 1 forma la primera parte del tour del futuro hijo, padre 2 termina el tour
    tripAuxiliar.push_back(listaHotelesP1[i]);
    tripAuxiliar.push_back(listaHotelesP2[i+1]);
    if posHP1 == -1 && checkTripFeasibility(tripAuxiliar, i) && !checkRepeatedHotels(listaHotelesP1Aux1, listaHotelesP2Aux1){
      posHP1 = static_cast<int>(i);
    }
    listaHotelesP1Aux1.erase(listaHotelesP1Aux1.begin());
    listaHotelesP2Aux1.erase(listaHotelesP2Aux1.end() - 1);

    tripAuxiliar.clear();


    //Padre 2 forma la primera parte del tour del futuro hijo, padre 1 termina el tour
    tripAuxiliar.push_back(listaHotelesP2[i]);
    tripAuxiliar.push_back(listaHotelesP1[i+1]);
    if posHP2 == -1 && checkTripFeasibility(tripAuxiliar, i) && !checkRepeatedHotels(listaHotelesP1Aux2, listaHotelesP2Aux2){
      posHP2 = static_cast<int>(i);
    }
    listaHotelesP2Aux2.erase(listaHotelesP2Aux2.begin());
    listaHotelesP1Aux2.erase(listaHotelesP1Aux2.end() - 1);

    tripAuxiliar.clear();
  }

  //Cruzamiento factible
  if posHP1 != -1 {
    vector<string> listaPOIsDisp;
    vector<string> listaPOIsDispPadres;
    for (int i=0; i<N; i++){
      listaPOIsDisp.push_back(to_string(i));
    }

    int numHotelesVistos = -1;
    for (size_t i=0; i<padre1->cromosoma.size(); i++){
      if numHotelesVistos == posHP1 {
        break;
      }

      if (padre1->cromosoma[i].find("H") == 0){
        numHotelesVistos++;
      } else {
        listaPOIsDisp.erase(remove(listaPOIsDisp.begin(), listaPOIsDisp.end(), padre1->cromosoma[i]), listaPOIsDisp.end());
      }
      
      hijo1->cromosoma.push_back(padre1->cromosoma[i]);
    }


    vector<string> tourFin;
    bool flagAdd = false;
    numHotelesVistos = -1
    for (size_t i=0; i<padre2->cromosoma.size(); i++){
      if (padre2->cromosoma[i].find("H") == 0){
        numHotelesVistos++;
        if numHotelesVistos == posHP1+1 {
          flagAdd = true;
        }
      } else {
        if !flagAdd {
          if (find(listaPOIsDispPadres.begin(), listaPOIsDispPadres.end(), padre2->cromosoma[i]) == listaPOIsDispPadres.end()) {
            listaPOIsDispPadres.push_back(padre2->cromosoma[i]);
            listaPOIsDisp.erase(remove(listaPOIsDisp.begin(), listaPOIsDisp.end(), padre2->cromosoma[i]), listaPOIsDisp.end());
          }
        }
      }
      if flagAdd {
        tourFin.push_back(padre2->cromosoma[i]);
        listaPOIsDisp.erase(remove(listaPOIsDisp.begin(), listaPOIsDisp.end(), padre2->cromosoma[i]), listaPOIsDisp.end());
      }
    }

    vector<string> TripCrossPoint;
    TripCrossPoint.push_back(listaHotelesP1[posHP1]);

    //Poblar con POIs de padres
    for (size_t i=0; i<listaPOIsDispPadres.size(); i++){
      TripCrossPoint.push_back(listaPOIsDispPadres[i]);
      TripCrossPoint.push_back(listaHotelesP2[posHP1+1]);
      if !(checkTripFeasibility(TripCrossPoint,posHP1)){
        TripCrossPoint.pop_back();
      }
      TripCrossPoint.pop_back();
    }
    
    //Poblar con POIs no visitados
    for (size_t i=0; i<listaPOIsDisp.size(); i++){
      TripCrossPoint.push_back(listaPOIsDisp[i]);
      TripCrossPoint.push_back(listaHotelesP2[posHP1+1]);
      if !(checkTripFeasibility(TripCrossPoint,posHP1)){
        TripCrossPoint.pop_back();
      }
      TripCrossPoint.pop_back();
    }

    //Añadir al final el trip (POIs de dicho trip)
    hijo1->cromosoma.insert(hijo1->cromosoma.end(), tourFin.begin()+1, tourFin.end()-1);

    //Añadir al final el resto del tour del otro padre
    hijo1->cromosoma.insert(hijo1->cromosoma.end(), tourFin.begin(), tourFin.end());

    //Borrar POIs duplicados
    vector<string> tourAux;
    for (size_t i=0; i<hijo1->cromosoma.size(); i++){
      if (find(tourAux.begin(), tourAux.end(), hijo1->cromosoma[i]) == tourAux.end()) {
        tourAux.push_back(hijo1->cromosoma[i]);
      }
    }
    hijo1->cromosoma = tourAux;
    cruzamientos++;
  }
}

void cruzar_individuos(individuo * padre1, individuo * padre2, individuo * hijo1, individuo * hijo2){
  if(float_rand(0.00, 1.00)<cr){
    onepointcrossover(padre1,padre2,hijo1,hijo2);
  } else {
    *hijo1 = *padre1;
    *hijo2 = *padre2;
  }
}

void cruzar_conjunto(conjunto & in, conjunto & out, int n){
  individuo * padre1;
  individuo * padre2;
  individuo hijo1, hijo2;
  for (vector<individuo>::iterator p = in.conj.begin (); p != in.conj.end (); p++){
    padre1 = &(*p);
    p++;
    if(p==in.conj.end ()){
      hijo1=*padre1;
      out.conj.push_back(hijo1);
      if(out.conj.size() == ps) {
          return;
      }
    } else{
      padre2 = &(*p);
      cruzar_individuos(padre1, padre2, &hijo1, &hijo2);
      out.conj.push_back(hijo1);
      out.conj.push_back(hijo2);
    }
  }

 //out = in;
 return;
}


void generateFeasibleSequenceOfHotels(const vector<string> &Hoteles, vector<string> &Tour){
  // Constantes para setear valor en caso de reset.
  const int iI = 0;
  const int cantHotelesQuitadosTourI = 1;
  const int cantHotelesQuitadosTripI = 1;

  vector<string> Trip;
  string Hinicio = "H0";
  string Htermino;
  Trip.push_back(Hinicio);
  Tour.push_back(Hinicio);
  int numHotel;
  vector<string> HotelesDisponiblesTrip(Hoteles); // Copia los elementos de Hoteles
  vector<string> HotelesDisponiblesTour(Hoteles); // Copia los elementos de Hoteles
  HotelesDisponiblesTour.erase(HotelesDisponiblesTour.begin());

  int i=iI;
  int cantHotelesQuitadosTour = cantHotelesQuitadosTourI;
  int cantHotelesQuitadosTrip = cantHotelesQuitadosTripI;

  bool tourFound = false;
  bool tripFound;
  bool flagAvoid = false;
  ostringstream auxiliaryString;
  while (!tourFound) {
    if (i==D){
      tourFound = true;
    } else {
      tripFound = false;
      HotelesDisponiblesTrip = HotelesDisponiblesTour;
      cantHotelesQuitadosTrip = cantHotelesQuitadosTour;
      Trip.clear();
      Trip.push_back(Hinicio);
      while(!tripFound){
        if (HotelesDisponiblesTrip.size()==0){
          i=iI;
          HotelesDisponiblesTour = Hoteles;
          HotelesDisponiblesTour.erase(HotelesDisponiblesTour.begin());
          HotelesDisponiblesTrip = HotelesDisponiblesTour;
          cantHotelesQuitadosTour = cantHotelesQuitadosTourI;
          cantHotelesQuitadosTrip = cantHotelesQuitadosTripI;
          Hinicio = "H0";
          Tour.clear();
          Tour.push_back(Hinicio);
          flagAvoid = true;
          auxiliaryString.str("");
          break;
        }
        
        if (i==D-1){
          numHotel = 0;
        } else {
          numHotel = int_rand(1, (HP1 - cantHotelesQuitadosTrip));
        }
        
        Htermino = HotelesDisponiblesTrip[numHotel];

        Trip.push_back(Htermino);
        
        if (!checkTripFeasibility(Trip, i)){
          if (i==D-1){
            i=iI;
            HotelesDisponiblesTour = Hoteles;
            HotelesDisponiblesTour.erase(HotelesDisponiblesTour.begin());
            HotelesDisponiblesTrip = HotelesDisponiblesTour;
            cantHotelesQuitadosTour = cantHotelesQuitadosTourI;
            cantHotelesQuitadosTrip = cantHotelesQuitadosTripI;
            Hinicio = "H0";
            Tour.clear();
            Tour.push_back(Hinicio);
            flagAvoid = true;
            auxiliaryString.str("");
            break;
          }
          HotelesDisponiblesTrip.erase(HotelesDisponiblesTrip.begin() + numHotel);
          cantHotelesQuitadosTrip++;
          Trip.pop_back();
        } else {
          if (debugGenerateFeasibleSequences) {
            auxiliaryString << i << endl;
            auxiliaryString << "numHotel: " << numHotel << endl;
            auxiliaryString << "Htermino: " << Htermino << endl;
            auxiliaryString << "Previo a eliminación" << endl;
            for (int k=0; k<HotelesDisponiblesTour.size(); k++){
              auxiliaryString << "HotelesDisponiblesTour[" << k << "]:" << HotelesDisponiblesTour[k] << endl;
            }
          }
          
          HotelesDisponiblesTour.erase(remove(HotelesDisponiblesTour.begin(), HotelesDisponiblesTour.end(), Htermino), HotelesDisponiblesTour.end());
          
          if (debugGenerateFeasibleSequences){
            auxiliaryString << "Post a eliminación" << endl;
            for (int k=0; k<HotelesDisponiblesTour.size(); k++){
              auxiliaryString << "HotelesDisponiblesTour[" << k << "]:" << HotelesDisponiblesTour[k] << endl;
            }
          }
          
          cantHotelesQuitadosTour++;
          Tour.push_back(Htermino);
          tripFound = true;
          flagAvoid = false;

          if (debugGenerateFeasibleSequences) {
            for (int l=0; l<Tour.size(); l++){
              auxiliaryString << "Tour[" << l << "]=" << Tour[l] << endl;
            }
            auxiliaryString << "-------------------------------------------" << endl;
          }
        }

      }
      if (!flagAvoid){
        Hinicio = Htermino;
        i++;
      }
    }
  }

  if (debugGenerateFeasibleSequences){
    cout << auxiliaryString.str();
    cout << "----------------------------------------------" << endl;

    for (int l=0; l<HotelesDisponiblesTour.size(); l++){
      cout << "H[" << l << "]=" << HotelesDisponiblesTour[l] << endl;
    }

    /*
    cout << "-------------------------------------------" << endl;
    for (int l=0; l<Tour.size(); l++){
      cout << "Tour[" << l << "]=" << Tour[l] << endl;
    }
    */
    cout << "Fin sequencia de hoteles" << endl;
    cout << "-------------------------------------------" << endl;
    cout << endl;
    getchar();
  }
}

void agregar_individuo_aleatorio (conjunto & c_temp) {
  int rand;
  individuo i_temp;
  i_temp.cromosoma.clear();
  i_temp.usado = true;

  vector<string> Hoteles;
  vector<string> POIsDispTour;
  vector<string> POIsDispTrip;
  vector<string> HotelesTour;

  // cout << "tinstancia: " << Tinstancia << endl;
  for(int i=0; i< HP1+N; i++) {
    if (i<HP1){
      Hoteles.push_back("H"+to_string(i));
    } else {
      POIsDispTour.push_back(to_string(i-HP1));
      POIsDispTrip.push_back(to_string(i-HP1));
    }
  }

  vector<string> Tour;
  vector<string> Trip;
  int cantidadPOIsAddTour = 0;
  int cantidadPOIsAddTrip = 0;
  int cantidadPOIsAddTourPastIteration = 0;
  int posPOI;
  bool isFeasibleAddPOIinTrip;
  generateFeasibleSequenceOfHotels(Hoteles, HotelesTour);
  i_temp.cromosoma.push_back(HotelesTour[0]);

  /*
  for (int l=0; l<HotelesTour.size(); l++){
    cout << "H[" << l << "]=" << HotelesTour[l] << endl;
  }
  getchar();
  */

  for (int i=0; i<D; i++){
    isFeasibleAddPOIinTrip = true;
    POIsDispTrip = POIsDispTour;
    cantidadPOIsAddTrip = cantidadPOIsAddTour;
    cantidadPOIsAddTourPastIteration = cantidadPOIsAddTour;
    // cout << "i: " << i << endl;
    while (isFeasibleAddPOIinTrip) {
      if (POIsDispTrip.size() == 0){
        break;
      }
      posPOI = int_rand(0, (N - cantidadPOIsAddTrip));
      // cout << "posPOI: "<< posPOI << endl;
      // cout << "tam: " << POIsDispTour.size() << endl;
      // cout << "POS: "<< POIsDispTour[posPOI] << endl;
      i_temp.cromosoma.push_back(POIsDispTour[posPOI]);
      i_temp.cromosoma.push_back(HotelesTour[i+1]);
      // cout << "pos: " << i + cantidadPOIsAddTour << endl;
      Trip.assign(i_temp.cromosoma.begin() + i + cantidadPOIsAddTourPastIteration, i_temp.cromosoma.end());
      /*
      for (int l=0; l<Trip.size(); l++){
        cout << "Trip[" << l << "]=" << Trip[l] << endl;
      }
      */
      if (checkTripFeasibility(Trip, i)){
        i_temp.cromosoma.pop_back();
        POIsDispTour.erase(POIsDispTour.begin() + posPOI);
        cantidadPOIsAddTour++;
      } else {
        i_temp.cromosoma.pop_back();
        i_temp.cromosoma.pop_back();
      }
      cantidadPOIsAddTrip++;
      POIsDispTrip.erase(POIsDispTrip.begin() + posPOI);
    } 
    i_temp.cromosoma.push_back(HotelesTour[i+1]);
  }

  /*
  string Hinicio = "H0";
  string Htermino;
  set.erase(set.begin() + 0);
  i_temp.cromosoma.push_back(Hinicio);
  int cantHotelesQuitados = 1;
  
  
  bool agregarPOI;
  int trip = 0;

  while (!solFactible) {
    Trip.push_back(Hinicio);


    if (trip==D-1){
      Htermino = "H1";
    } else {
      Htermino = "H"+to_string(int_rand(1, (HP1 - cantHotelesQuitados)));
    }
    Trip.push_back(Htermino);
    set.erase(set.begin() + getPos(Htermino));
    cantHotelesQuitados++;

    if (checkTripFeasibility(Trip, trip)){
      i_temp.cromosoma.push_back(POI);
    }

    agregarPOI = true;
    int POIpos;
    string POI;
    while (agregarPOI) {
      if (set.size() == 0){
        break;
      }
      POIpos = int_rand(HP1 - cantHotelesQuitados, set.size());
      // cout << "POIpos: " << POIpos << endl;
      POI = set[POIpos];
      // cout << "set[POIpos]: " << set[POIpos] << endl;
      set.erase(set.begin() + getPos(POI));
      // POIs.erase(set.begin() + stoi(POI));
      Trip.insert(Trip.end()-1,POI);
      if (checkTripFeasibility(Trip, trip)){
        i_temp.cromosoma.insert(Trip.end()-1,POI);
      } else {
        agregarPOI = false;
      }
    }

    i_temp.cromosoma.push_back(Htermino);
    Hinicio = Htermino;
    if (trip == 0){
      for (int i=0; i<Trip.size(); i++) {
        cout << Trip[i] << endl;
      }
    }
    Trip.clear();
    trip++;
  }*/

  //calculo de la aptitud del individuo
  calcular_aptitud(&i_temp);
  c_temp.conj.push_back(i_temp);
  return;
}


void crear_poblacion_inicial(conjunto &poblacion, int size){
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
  readConfiguration(archivo_configuracion);
  Tinstancia = (D+1)+N;                       //Tamaño de cromosomas
  calculateDistances();
  optimo_encontrado = false;
  evaluaciones = 0;

  //medicion de tiempo
  Inicio=time(NULL);

  //semilla aleatoria
  srand48 (semilla);

  //creacion de poblacion inicial aleatoria
  // inicializar_archivo_convergencia();
  conjunto poblacion ((char*)"poblacion");
  crear_poblacion_inicial(poblacion, ps);

  if(debug) {
    cout<<"POBLACION INICIAL"<<endl;
    cout<<poblacion;
    getchar();
  }

  //contador de mutaciones
  mutaciones=0;
  cruzamientos=0;
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
    
    seleccionar_conjunto(poblacion, seleccionados, ps);
    if(debug){
      cout<<"POBLACION SELECCIONADA"<<endl;
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
  }

  salir();

  return 0;
}