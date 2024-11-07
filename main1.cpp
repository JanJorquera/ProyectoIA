#include "includes.h"
#include "globales.h"

bool debug = false;

void inicializar_archivo_convergencia(void)
{
  FILE * conv;
  conv = fopen (archivo_convergencia, "w");
  fclose(conv);
  return;
}

void escribir_en_archivo_convergencia(void)
{
  FILE * conv;
  conv = fopen (archivo_convergencia, "a");
  fprintf(conv, "%d %d\n", iteracion, optimo.aptitud);
  fclose(conv);
}


void seleccionar_dos_de_tres_puntos_de_corte(int *corte1, int *corte2, int *corte3)
{
  int Puntos=3;
  int c[Puntos], aux;
  c[0]=*corte1;
  c[1]=c[0];
  c[2]=c[0];
  
  do{
    c[1]=int_rand(0, Tinstancia);
  }while(c[1]==c[0]);
  do{
    c[2]=int_rand(0, Tinstancia);
  }while((c[2]==c[0]) || (c[2]==c[1]));
  
  //ordenar los puntos de corte
  for(int i=0; i<(Puntos-1); i++)
    {
      for(int j=i+1; j<Puntos; j++)
	{
	  if(c[i] > c[j])
	    {
	      aux = c[i];
	      c[i] = c[j];
	      c[j] = aux;
	    }
	}
    }
  *corte1 = c[0];
  *corte2 = c[1];
  *corte3 = c[2];
  return;
}

void seleccionar_dos_puntos_de_corte(int *corte1, int *corte2)
{
  //seleccionar dos puntos de corte diferentes entre variables
  int c1, c2, aux;
  c1=int_rand(0, Tinstancia); //entre 0 y tamanno instancia-1
  do{
    c2=int_rand(0, Tinstancia);
  }while(c1==c2);

  //revisar condicion de c1 < c2
  if(c1 > c2){
    aux = c2;
    c2 = c1;
    c1=aux;
  }

  *corte1 = c1;
  *corte2 = c2;
  return;
}


void seleccionar_uno_de_dos_puntos_de_corte(int * corte1, int * corte2)
{
  int c1, c2, aux;
  c1 = *corte1;
  do{
    c2=int_rand(0, Tinstancia);
  }while(c1==c2);
  //revisar condicion de c1 < c2
  if(c1 > c2){
    aux = c2;
    c2 = c1;
    c1=aux;
  }

  *corte1 = c1;
  *corte2 = c2;
  return;
}

//Seleccion de individuos//
individuo * roulette_wheel (conjunto & c_temp)
{
  int i = 0;
  float rand;
  float acumulado = 0.00, suma = 0.00;
  individuo * i_temp;

  //aptitud temporal de cada individuo (problema de minimizacion)
  float aptitud_temporal[ps];
  
  sort(c_temp.conj.begin(), c_temp.conj.end());
  //se supone poblacion ordenada
  float maximo = c_temp.conj.back().aptitud;
  float minimo = c_temp.conj.front().aptitud;
  
  for (vector<individuo>::iterator p = c_temp.conj.begin (); p != c_temp.conj.end (); p++)
    {
      //calculo de la aptitud temporal de cada individuo de la poblacion
      aptitud_temporal[i] = minimo + maximo - (p->aptitud); //version2.0
      suma += aptitud_temporal[i];
      i++;
    }
  //la suma de la aptitud de todos los individuos no puede ser cero
  if(suma == 0)
    {
      cout<<"ERROR: Condicion extraña en seleccionar individuo"<<endl;
      i_temp = (&(c_temp.conj.front()));
      return i_temp;
    }
  //numero aleatorio para la ruleta
  rand = float_rand(0.00, suma);
  i=0;
  
  //busqueda del individuo en la ruleta
  for (vector<individuo>::iterator p = c_temp.conj.begin (); p != c_temp.conj.end (); p++)
    {
      acumulado += aptitud_temporal[i];
      i++;
      if (acumulado > rand)
	{
	  i_temp = &(*p);
	  return i_temp;
	}
    }
  //en caso de ser el ultimo individuo de la poblacion
  i_temp = (&(c_temp.conj.back()));
  return i_temp;
}

individuo * ranking (conjunto & c_temp)
{
  individuo * i_temp;
  int ranking = 0;
  int rand, acumulado = 0;
  
  ranking = c_temp.conj.size();
  int total = (c_temp.conj.size()*(c_temp.conj.size()+1))/2;
  rand = int_rand(0, total+1);
  sort(c_temp.conj.begin(), c_temp.conj.end());
  
  for (vector<individuo>::iterator p = c_temp.conj.begin (); p != c_temp.conj.end (); p++)
    {
      acumulado += ranking;
      if (acumulado >= rand)
	{
	  i_temp = &*p;
	  return i_temp;
	}
      ranking--;
    }
  //en caso de ser el ultimo individuo de la poblacion
  i_temp = &(c_temp.conj.back());
  return i_temp;
}

individuo * tournament (conjunto & c_temp)
{
  int rand1, rand2;
  individuo * i_temp;
  
  sort(c_temp.conj.begin(), c_temp.conj.end());
  
  //numero aleatorio para la ruleta
  rand1 = int_rand(0, c_temp.conj.size());
  rand2 = int_rand(0, c_temp.conj.size());
  int i=0;
  int buscar;

  if((c_temp.conj[rand1].aptitud) < (c_temp.conj[rand2].aptitud))
    {
      buscar = rand1;
    }
  else
    {
      buscar = rand2;
    }
  
  //busqueda del individuo en la ruleta
  for (vector<individuo>::iterator p = c_temp.conj.begin (); p != c_temp.conj.end (); p++)
    {
      if(i==buscar){
	i_temp = &(*p);
	return i_temp;
      }
      i++;
    }
  //en caso de ser el ultimo individuo de la poblacion
  i_temp = (&(c_temp.conj.back()));
  return i_temp;
}

individuo * seleccionar_individuo (conjunto & c_temp)
{
  individuo * i_temp;
  switch(so)
    {
      case ROULETTE:
	{
	  i_temp = roulette_wheel(c_temp);
	  return i_temp;
	}
    case TOURNAMENT:
      {
	i_temp = tournament(c_temp);
	return i_temp;
      }
    case RANKING:
      {
	i_temp = ranking(c_temp);
	return i_temp;
      }
    default:
      {
	cout<<"ERROR: No se reconoce tipo de seleccion"<<endl;
	salir();
      } 
   }
}

void seleccionar_conjunto(conjunto & in, conjunto & out, int n)
{
  individuo * p_i_temp;
  for(int i=0; i<n; i++)
    {
      individuo i_temp;
      p_i_temp = seleccionar_individuo(in);
      i_temp = *p_i_temp;
      out.conj.push_back(i_temp);
    }
}

//Mutacion de individuos//
void mutacionSWAP(individuo * padre, individuo * hijo)
{
  *hijo = *padre;
  int pto1, pto2;
  //seleccionar dos puntos en el individuo
  for(int i=0; i<Tinstancia; i++)
  {
    if(float_rand(0.00, 1.00)<mr)
    {
      pto1=i;
      pto2=int_rand(0, Tinstancia);
      int aux = hijo->cromosoma[pto1];
      hijo->cromosoma[pto1] = hijo->cromosoma[pto2];
      hijo->cromosoma[pto2] = aux;
    }
  }
  //cout<<endl;
  validar_permutacion (hijo);
  hijo->usado = true;
  calcular_aptitud(hijo);
  mutaciones++;
  return;
}

void mutacion2OPT(individuo * padre, individuo * hijo)
{
  int aux;
  //copiar la informacion del padre en el hijo
  *hijo=*padre;
  int corte1, corte2;
  
  for(int i=0; i<Tinstancia; i++)
  {
    if(float_rand(0.00, 1.00) < mr)
    {
      corte1 = i;
      seleccionar_uno_de_dos_puntos_de_corte(&corte1, &corte2);
      int prom = (int)ceil((corte2 - corte1)/2.00);
      //cout<<endl<<"corte1:"<<corte1<<" y corte 2: "<<corte2<<endl;
      //Girar la informacion entre los cortes
      int j=0;
      while(j<prom)
      {
	aux = hijo->cromosoma[corte1+j];
	hijo->cromosoma[corte1+j] = hijo->cromosoma[corte2-j];
	hijo->cromosoma[corte2-j] = aux;
	j++;
      }
    }
  }
  //Validar permutacion y marcar como usado
  validar_permutacion(hijo);
  hijo->usado = true;
  calcular_aptitud(hijo);
  mutaciones++;
  return;
}

void agregar_sub_tour(individuo * destino, individuo * origen, int inicio, int fin)
{
  if(((inicio>Tinstancia-1)) || ((fin>Tinstancia-1)))
    return;
  //if(inicio==fin)
  //return;
  //cout<<"agregando sub tour"<<endl;
  int i=0;
  while((destino->cromosoma[i]) != (-1))
    {
      i++;
      if(i==(Tinstancia-1))
	break;
    }
  if(inicio<fin)//en orden
    {
      for(int j=inicio; j<=fin; j++,i++)
	{
	  destino->cromosoma[i] = origen->cromosoma[j];
	}     
    }
  else //desordenado
    {
      for(int j=inicio; j>=fin; j--,i++)
	{
	  destino->cromosoma[i] = origen->cromosoma[j];
	}     
    }
  return;
}

void mutacion3OPT(individuo * padre, individuo * hijo)
{
  int corte1, corte2, corte3;
  *hijo = *padre;
  for(int i=0; i<Tinstancia; i++)
  {
    if(float_rand(0.00, 1.00)<mr)
    {
      hijo->cromosoma.assign(Tinstancia, (-1));
      corte1=i;
      //seleccionar 3 puntos de corte diferentes entre variables  
      seleccionar_dos_de_tres_puntos_de_corte(&corte1, &corte2, &corte3);
      //incluir en el hijo la informacion inicial
      agregar_sub_tour(hijo, padre, 0, corte1);
      //Numero aleatorio entre 0 y 7
      int combinacion = int_rand(0, 8); //8 posibilidades en total
    
      switch(combinacion)
      {
	case 0:
	{
	  agregar_sub_tour(hijo, padre, corte1+1, corte2);
	  agregar_sub_tour(hijo, padre, corte2+1, corte3);
	  break;
	}
	case 1:
	{
	  agregar_sub_tour(hijo, padre, corte1+1, corte2);
	  agregar_sub_tour(hijo, padre, corte3, corte2+1);
	  break;
	}
	case 2:
	{
	  agregar_sub_tour(hijo, padre, corte2, corte1+1);
	  agregar_sub_tour(hijo, padre, corte2+1, corte3);
	  break;
	}
	case 3:
	{
	  agregar_sub_tour(hijo, padre, corte2, corte1+1);
	  agregar_sub_tour(hijo, padre, corte3, corte2+1);
	  break;
	}
	case 4:
	{
	  agregar_sub_tour(hijo, padre, corte2+1, corte3);
	  agregar_sub_tour(hijo, padre, corte1+1, corte2);
	  break;
	}
	case 5:
	{
	  agregar_sub_tour(hijo, padre, corte3, corte2+1);
	  agregar_sub_tour(hijo, padre, corte1+1, corte2);
	  break;
	}
	case 6:
	{
	  agregar_sub_tour(hijo, padre, corte2+1 , corte3);
	  agregar_sub_tour(hijo, padre, corte2, corte1+1);
	  break;
	}
	case 7:
	{
	  agregar_sub_tour(hijo, padre, corte3, corte2+1);
	  agregar_sub_tour(hijo, padre, corte2, corte1+1);
	  break;
	}
      }
      agregar_sub_tour(hijo, padre, corte3+1, (Tinstancia-1));
    }
  }
  validar_permutacion (hijo);
  hijo->usado = true;
  calcular_aptitud(hijo);
  mutaciones++;
  return;
}

void mutar_individuo(individuo * padre, individuo * hijo, float mr)
{
  *hijo = *padre;
    switch(mo)
    {
      case DOSOPT:
      {
	//cout<<endl<<"2OPT: "<<mr;
	//cout<<endl<<*padre;
	mutacion2OPT(padre, hijo);
	//cout<<endl<<*hijo;
	//getchar();
	return;
      }
      case TRESOPT:
      {
	mutacion3OPT(padre, hijo);
	return;
      }
      case SWAP:
      {
	mutacionSWAP(padre, hijo);
	return;
      }
      default:
      {
	cout<<"ERROR: No se reconoce tipo de mutacion"<<endl;
	salir();
      }
    }
    //}
  return;
}

void mutar_conjunto(conjunto & in, conjunto & out, float mr)
{
  individuo * p_seleccionado;
  for (vector<individuo>::iterator p = in.conj.begin (); p != in.conj.end (); p++)
    {
      individuo mutado;
      p_seleccionado = &*p;
      mutar_individuo(p_seleccionado, &mutado, mr);
      calcular_aptitud(&mutado);
      out.conj.push_back(mutado);
    }
  return;
}


void guardar_optimo_encontrado (int aptitud_optimo, individuo temp)
{
  //recuerda el optimo encontrado
  optimo.aptitud = aptitud_optimo;
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

void escribir_en_archivo_resultados(individuo * optimo)
{
  //escritura en el archivo de resultados
  //calculo de tiempo de ejecucion del algoritmo
  //time_t Fin;
  //Fin=time(NULL);
  FILE * res;  
  res = fopen (archivo_resultados, "a");
  //calculo de evalauciones para tuning
  /*if(optimo_encontrado)
    fprintf(res, "%d\n", evaluaciones);
  else
    fprintf(res, "%d\n", maximo_evaluaciones);
  */

  //fprintf(res, "%d %d %d %d %d %d %d\n", optimo->aptitud, evaluacion_opt, evaluaciones, iteracion_opt, iteracion, (int)(Fin_opt-Inicio), (int)(time(NULL)- Inicio));
  float distancia_relativa = 100*(optimo->aptitud - optimo_global.aptitud)/((float)(optimo_global.aptitud));
  fprintf(res, "%d \n", optimo->aptitud);
  //fprintf(res, "%.2f \n", distancia_relativa);
  //SPOT
  printf("%d \n", optimo->aptitud);
  //printf("%.2f \n", distancia_relativa);
  fclose(res);
  return;
}

void calcular_aptitud(individuo * temp)
{
  //Reevaluar solo los individuos con cambios
  if(temp->usado==false) 
   {
     return;  
   }
  int apt=0;
  //calcular el costo del tour
  for(int i=0; i<(Tinstancia-1); i++)
  {
    apt += (int)distancia[temp->cromosoma[i]][temp->cromosoma[i+1]];
  }
  apt += (int)distancia[temp->cromosoma[Tinstancia-1]][temp->cromosoma[0]];
  
  temp->aptitud = apt;

  if(iteracion>0){
    /*if(apt < optimo_global.aptitud)
      {
	cout<<"ERROR: Funcion de aptitud invalida"<<endl; salir();
      }*/
    //OJO!!!, la condicion de óptimo encontrado ha cambiado
    if(apt == optimo_global.aptitud)      
    //if(distancia_relativa < 10.00)
      {
       guardar_optimo_encontrado(apt, *temp);
      }
  }

  //conteo de la cantidad de evaluaciones realizadas en el algoritmo
  evaluaciones++;
  //chequeo de maximo de evaluaciones
  if(evaluaciones >= maximo_evaluaciones)
    {
      salir();
    }
  return;
}

void validar_permutacion(individuo * temp)
{
  int set[Tinstancia];
  for(int i=0; i<Tinstancia;i++)
    {
      set[i]= 1; 
    }
  for(int i=0; i<Tinstancia;i++)
    {
      if(set[temp->cromosoma[i]] == 0)
	{
	  cout<<"ERROR: Individuo invalido"<<endl; salir();
	}
      set[temp->cromosoma[i]] = 0;
    }
  return;
}

float conv_rad(float coord)
{
  int int_coord = coord;
  double dif = coord - int_coord;
  return (PI*(int_coord + 5.0*dif/3.0)/180.0);
}

void leer_archivo_configuracion(void)
{
  char instancia[1000];
  //lectura de archivo de configuracion
  FILE *configuracion;
  char buf[100];
  int i, j;

  configuracion = fopen(archivo_configuracion, "r");
  int rt=fscanf(configuracion,"%s", buf);
  while ( strcmp("NODE_COORD_SECTION", buf) != 0 ) 
    {
      if( strcmp("EDGE_WEIGHT_TYPE", buf) == 0 ) 
	{
	  rt=fscanf(configuracion,"%s", buf); //los dos puntos que estan separados
	  rt=fscanf(configuracion,"%s", buf);
	  if ( strcmp("GEO", buf) == 0 ) 
	    {
	      tipodistancia = GEO;
	    }
	  else if ( strcmp("EUC_2D", buf) == 0 ) 
	    {
	      tipodistancia = EUC;
	    }
	  buf[0]=0;
	}
      else if ( strcmp("EDGE_WEIGHT_TYPE:", buf) == 0 ) 
	{
	  int rt=fscanf(configuracion,"%s", buf);
	  if ( strcmp("GEO", buf) == 0 ) 
	    {
	      tipodistancia = GEO;
	    }
	  else if ( strcmp("EUC_2D", buf) == 0 ) 
	    {
	      tipodistancia = EUC;
	    }
	  buf[0]=0;
	}
      else if( strcmp("DIMENSION", buf) == 0 )
	{
	  rt=fscanf(configuracion,"%s", buf);//los dos puntos que estan separados
	  rt=fscanf(configuracion, "%d", &Tinstancia);
	  buf[0]=0;
	}
      else if ( strcmp("DIMENSION:", buf) == 0 ) 
	{
	  rt=fscanf(configuracion, "%d", &Tinstancia);
	  buf[0]=0;
	}
      else if( strcmp("NAME", buf) == 0 )
	{
	  buf[0]=0;
	}
      else if( strcmp("NAME:", buf) == 0 ) 
	{
	  buf[0]=0;
	}
      rt=fscanf(configuracion,"%s", buf);
    }

  if( strcmp("NODE_COORD_SECTION", buf) == 0 )
    {
      if(Tinstancia == 0){
        cout<<"ERROR: Problemas en la lectura del tamano de la instancia"<<endl;
        salir();
      }

      //Reserva de espacio para el arreglo de coordenadas y la matriz de distancia
      coordenadas = new coordenada[Tinstancia];
      distancia = new int*[Tinstancia];
      for( int i = 0 ; i < Tinstancia ; i++ )
        {
          distancia[i] = new int [Tinstancia];
        }
      //Lectura de las coordenadas de cada punto
      int id;
      float x, y;
      int contador=0;
      while(contador < Tinstancia)
        {
          int rt=fscanf(configuracion, "%d %f %f\n", &id, &x, &y);
          coordenadas[id-1].x = x;
          coordenadas[id-1].y = y;
          contador++;
        }
    }

  //Calculo de la matriz de distancias
  for(int i=0; i<(Tinstancia-1);i++)
    {
      for(int j=i+1; j<Tinstancia; j++)
	{
	  if( tipodistancia == EUC )
	    {
	      distancia[i][j] = round(sqrt(pow((coordenadas[i].x - coordenadas[j].x),2) + pow((coordenadas[i].y - coordenadas[j].y),2)));
	    }
            else if( tipodistancia == GEO){
              float RRR = 6378.388;
              float latitud1 = conv_rad(coordenadas[i].x);
              float longitud1 = conv_rad(coordenadas[i].y);
              float latitud2 = conv_rad(coordenadas[j].x);
              float longitud2 = conv_rad(coordenadas[j].y);
              float q1 = cos(longitud1-longitud2);
              float q2 = cos(latitud1-latitud2);
              float q3 = cos(latitud1+latitud2);
              distancia[i][j] = (int) (RRR*acos(0.5*((1.0+q1)*q2 - (1.0-q1)*q3))+1.0);
          }
          distancia[j][i] = distancia[i][j];
        }
    }
  
  strcpy(instancia, archivo_configuracion);
  int largo = strlen(instancia);
  instancia[largo-4] = '\0';
  strcat (instancia,".opt.tour");

  FILE *lectura_optimo;
  lectura_optimo = fopen(instancia, "r");
  if(lectura_optimo == NULL)
    {
      cout<<"ERROR: Problemas en la lectura del archivo de tour optimo"<<endl;
      salir();
    }

  do{
    int rt=fscanf(lectura_optimo,"%s", buf); 
  }while (strcmp("TOUR_SECTION", buf) != 0);

  int tmp;
  for(int i=0; i< Tinstancia; i++)
    {
      int rt=fscanf(lectura_optimo, "%d", &tmp);
      //Estas dos variables son globales, declaradas en el archivo globales.h, por alguna razón ellas no usan el constructor y no reservan el espacio para cromosoma correctamente.
      optimo_global.cromosoma.push_back(tmp-1);
      optimo.cromosoma.push_back(-1);
    }
  optimo_global.usado=true;
  calcular_aptitud(&optimo_global);
  
  fclose(lectura_optimo);
  fclose(configuracion);
  return;
}

void agregar_individuo_aleatorio (conjunto & c_temp)
{
  int rand;
  individuo i_temp;
  i_temp.usado = true;
  //creacion de la permutacion aleatoria
  int set[Tinstancia];
  for(int i=0; i< Tinstancia; i++)
    {
      set[i] = i;
    }

  for (int i = 0; i < Tinstancia; i++)
    {
      rand = int_rand (0, (Tinstancia - i)); // Un elemento del arreglo (entre 0 y Tinstancia-1 la primera vez y asi...)
      i_temp.cromosoma[i]=set[rand]; 
      set[rand] = set[Tinstancia-i-1]; 
    }
  //calculo de la aptitud del individuo
  calcular_aptitud (&i_temp);
  c_temp.conj.push_back (i_temp);
  return;
}

void crear_poblacion_inicial (conjunto & c_temp, int size)
{
  for (int i=0; i<size; i++)
    {
      agregar_individuo_aleatorio(c_temp);
    }
  return;
}

int int_rand (int a, int b)
{
  //generacion de numeros aleatorios entre a y b-1
  int retorno = 0;
  if (a < b)
    { 
      //retorno cuando a < b
      retorno = (int) ((b - a) * drand48 ());
      retorno = retorno + a;
    }
  else
    {
      //retorno cuando b <= a
      retorno = (int) ((a - b) * drand48 ());
      retorno = retorno + b;
    }
  return retorno;
}

float float_rand (float a, float b)
{
  //generacion de numeros aleatorios entre a y b-1
  float retorno = 0.00;
  if (a < b)
    {
      //retorno cuando a < b
      retorno = ((b - a) * drand48 ());
      retorno = retorno + a;
    }
  else
    {
      //retorno cuando b <= a
      retorno = ((a - b) * drand48 ());
      retorno = retorno + b;
    }
  return retorno;
}

void salir(void)
{
  escribir_en_archivo_resultados(&optimo);
  //salida sin error
  exit(0);
  return;
}

void guardar_optimo(conjunto & c_temp)
{
  sort(c_temp.conj.begin(), c_temp.conj.end());
  if(c_temp.conj.front().aptitud < optimo.aptitud)
    {
      optimo = c_temp.conj.front();
      iteracion_opt = iteracion;
      evaluacion_opt = evaluaciones;
      Fin_opt = time(NULL);
      //cout<<iteracion<<" "<< optimo.aptitud<<endl;
      escribir_en_archivo_convergencia();
    }
  //Si se cumplen XX evaluaciones sin cambio se acaba
  if((evaluaciones - evaluacion_opt)>1000000)
  {
  	salir();
  }
  return;
}

int esta_presente(int valor, individuo * individuo_buscar){
  for(int i=0; i<Tinstancia;i++){                          
    if(individuo_buscar->cromosoma[i] == valor)            
      return true;                                         
  }                                                        
  return false;                                            
}                                                          

void ordercrossover(individuo * padre1, individuo * padre2, individuo * hijo1, individuo * hijo2)
{
  hijo1->cromosoma.assign(Tinstancia, (-1));
  hijo2->cromosoma.assign(Tinstancia, (-1));
  //seleccionar dos puntos de corte diferentes entre variables
  int corte1, corte2;                                           
  seleccionar_dos_puntos_de_corte(&corte1, &corte2);            
  //copia de los subtours en los hijos
  for(int i=corte1; i<=corte2; i++){  
    hijo1->cromosoma[i] = padre2->cromosoma[i];
    hijo2->cromosoma[i] = padre1->cromosoma[i];
  }                                            
  //hijo1
  int j=corte2;
  for(int i=corte2+1; i<Tinstancia;i++){
    do{                                 
      j++;                              
      j=j%Tinstancia; 
    }while(esta_presente(padre1->cromosoma[j], hijo1));
    hijo1->cromosoma[i] = padre1->cromosoma[j];        
  }
  for(int i=0; i<corte1;i++){                          
    do{                                                
      j++;                                             
      j=j%Tinstancia;                                  
    }while(esta_presente(padre1->cromosoma[j], hijo1));
    hijo1->cromosoma[i] = padre1->cromosoma[j];        
  }
  //hijo2                                              
  j=corte2;                                            
  for(int i=corte2+1; i<Tinstancia;i++){               
    do{                                                
      j++;                                             
      j=j%Tinstancia;                                  
    }while(esta_presente(padre2->cromosoma[j], hijo2));
    hijo2->cromosoma[i] = padre2->cromosoma[j];        
  }
  for(int i=0; i<corte1;i++){
    do{
      j++;
      j=j%Tinstancia;
    }while(esta_presente(padre2->cromosoma[j], hijo2));
    hijo2->cromosoma[i] = padre2->cromosoma[j];
  }
  validar_permutacion (hijo1);
  validar_permutacion (hijo2);
  cruzamientos++;
  hijo1->usado = true;
  hijo2->usado = true;
  calcular_aptitud (hijo1);
  calcular_aptitud (hijo2);
  return;                                                              
}

struct par
{         
  int v1, v2; 
};            

void mostrar_mapa(par *pares, int tamano, int sentido)
{
  cout<<"####################### Mapa actual #################################"<<endl;
  for(int i=0; i<tamano; i++)                                                         
    {                                                                                 
      if(sentido == 0)                                                                
        cout<< pares[i].v1 + 1 <<"->"<<pares[i].v2 + 1 <<endl;                        
      if(sentido==1)                                                                  
        cout<< pares[i].v2 + 1 <<"->"<<pares[i].v1 + 1 <<endl;                        
                                                                                      
    }                                                                                 
  cout<<"####################################################################"<<endl; 
  return;                                                                             
}                                                                                     

int buscar_en_mapa(par *pares, int tamano, int valor, int sentido)
{                                                                 
  int respuesta = -1;                                             
  for(int i=0; i<tamano; i++)                                     
    {                                                             
      if(sentido == 0)                                            
        if(pares[i].v1 == valor){                                 
          respuesta = pares[i].v2;                                
        }                                                         
      if(sentido==1)                                              
        if(pares[i].v2 == valor){                                 
          respuesta = pares[i].v1;                                
        }                                                         
    }                                                             
  if(respuesta == (-1))                                           
    return valor;                                                 
  else                                                            
    valor = buscar_en_mapa(pares, tamano, respuesta, sentido);    
  return valor;                                                   
}                                                                 

void partiallymappedcrossover(individuo * padre1, individuo * padre2, individuo * hijo1, individuo * hijo2)
{
  hijo1->cromosoma.assign(Tinstancia, (-1));
  hijo2->cromosoma.assign(Tinstancia, (-1));
  //seleccionar dos puntos de corte diferentes entre variables
  int corte1, corte2;
  seleccionar_dos_puntos_de_corte(&corte1, &corte2);
  int tamano = abs(corte2 - corte1) + 1;
  //mapeo                               
  par mapa[tamano];
  for(int i=0; i<=tamano; i++)
    {                         
      mapa[i].v1 = padre1->cromosoma[i+corte1];
      mapa[i].v2 = padre2->cromosoma[i+corte1];
    }
  for(int i=corte1; i<=corte2; i++)
    {                              
      hijo1->cromosoma[i]=padre2->cromosoma[i];
      hijo2->cromosoma[i]=padre1->cromosoma[i];
    }
  int i=0;
  while(i<Tinstancia)
    {                
      if(i == corte1){
        if(corte2==Tinstancia-1)
          break;                
        else                    
          i=corte2+1;           
      }                         
      if(esta_presente(padre1->cromosoma[i], hijo1))
        hijo1->cromosoma[i] = buscar_en_mapa(mapa, tamano, padre1->cromosoma[i],1);
      else                                                                         
        hijo1->cromosoma[i] = padre1->cromosoma[i];                                

      if(esta_presente(padre2->cromosoma[i], hijo2))
        hijo2->cromosoma[i] = buscar_en_mapa(mapa, tamano, padre2->cromosoma[i],0);
      else                                                                         
        hijo2->cromosoma[i] = padre2->cromosoma[i];                                
      i++;
    }     

  //Validacion, preparacion para calculo de aptitud y conteo de operaciones
  validar_permutacion (hijo1);
  validar_permutacion (hijo2);
  cruzamientos++;
  hijo1->usado = true;
  hijo2->usado = true;
  calcular_aptitud (hijo1);
  calcular_aptitud (hijo2);
  return;                                                              
}

void llenar_restante(individuo * destino, individuo * origen)
{                                                            
  int i=0, j=0;                                              
  while(i < Tinstancia)                                      
    {
      while(destino->cromosoma[i] == (-1))
        {                   
          if(!esta_presente(origen->cromosoma[j], destino))
            {  
              destino->cromosoma[i] = origen->cromosoma[j];
            }
          j++;
        }
      i++;
    }
}

void positionbasedcrossover(individuo * padre1, individuo * padre2, individuo * hijo1, individuo * hijo2)
{
  hijo1->cromosoma.assign(Tinstancia, (-1));
  hijo2->cromosoma.assign(Tinstancia, (-1));

  for(int i=0; i<Tinstancia;i++)
    {                           
      if(int_rand (0,101) < 50) 
        {                       
          hijo1->cromosoma[i] = padre1->cromosoma[i];
          hijo2->cromosoma[i] = padre2->cromosoma[i];
        }                                            
  }                                                  

  llenar_restante(hijo1, padre2);
  llenar_restante(hijo2, padre1);

  //Validacion, preparacion para calculo de aptitud y conteo de operaciones
  validar_permutacion (hijo1);                                             
  validar_permutacion (hijo2);                                             
  cruzamientos++;                                                                  
  hijo1->usado = true;                                                     
  hijo2->usado = true;                                                     

  calcular_aptitud (hijo1);
  calcular_aptitud (hijo2);
  return;
}


void cruzar_individuos(individuo * padre1, individuo * padre2, individuo * hijo1, individuo * hijo2)
{
  if(float_rand(0.00, 1.00)<cr)
    {
      switch(co)
	{
	  case OX:
	    {
	      ordercrossover(padre1, padre2, hijo1, hijo2);
	      return;
	    }
	  case PMX:
	    {
	      partiallymappedcrossover(padre1, padre2, hijo1, hijo2);
	      return;
	    }
	  case PBX:
	    {
	      positionbasedcrossover(padre1, padre2, hijo1, hijo2);
	      return;
	    }
	  default:
	    {
	      cout<<"ERROR: No se reconoce tipo de cruzamiento"<<endl;
	      salir();
	    }
	}
   }
  else
  {
    *hijo1 = *padre1;
    *hijo2 = *padre2;
  }
}

void cruzar_conjunto(conjunto & in, conjunto & out, int n)
{
  individuo * padre1;
  individuo * padre2;
  individuo hijo1, hijo2;
  for (vector<individuo>::iterator p = in.conj.begin (); p != in.conj.end (); p++)
    {
      padre1 = &(*p);
      p++;
      if(p==in.conj.end ())
      {
	hijo1=*padre1;
	out.conj.push_back(hijo1);
	if(out.conj.size() == ps)
	  {
	    return;
	  }
      }
      else{
	padre2 = &(*p);
	cruzar_individuos(padre1, padre2, &hijo1, &hijo2);
	out.conj.push_back(hijo1);
	out.conj.push_back(hijo2);
       }
    }
}

static void control_c (int signo)
{
  //escribir_en_archivo_resultados(&optimo);
  exit (1);
}

int Leer_entradas(int argc, char **argv)
{
  //archivo con la instancia del problema
  archivo_configuracion = (char *)(argv[1]);
  //archivo donde escribir los resultados de la ejecucion
  archivo_resultados = (char *)(argv[2]);
  
  //PARAMETROS
  so = atoi(argv[3]); //selection operator 
  co = atoi(argv[4]); //crossover operator
  mo = atoi(argv[5]); //mutation operator
  eo = 1;
  cr = atof(argv[6]); //crossover rate
  mr = atof(argv[7]); //mutation rate
  ps = atoi(argv[8]); //population size
  if(debug)
  {
    printf("so: %d, mo: %d, co: %d, eo: %d, mr: %.2f, cr: %.2f, ps: %d\n", so, mo, co, eo, mr, cr, ps);
    //getchar();
  }
  //BUDGET
  max_iter = (int)(atof(argv[9]));
  maximo_evaluaciones = max_iter;
  //SEMILLA
  semilla = atoi (argv[10]);
  return 1;
}


int main (int argc, char *argv[])
{
  struct timeval hora;
  if (signal (SIGINT, control_c) == SIG_ERR)
    printf ("can't catch SIGTERM");
  
  //lectura de parametros
  if(!Leer_entradas(argc,argv))
    {
      cout<<"Problemas en la lectura de los parametros";
      exit(1);
    }

  //lectura de instancia
  iteracion = -1;
  leer_archivo_configuracion();
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

  if(debug)
    {
      cout<<poblacion;
      getchar();
    }
  //contador de mutaciones
  mutaciones=0;
  //int evaluaciones_ant = 0;
  //int prom_it;
  for (iteracion = 0; iteracion < max_iter; iteracion++)
    {
      time_t ini_it=time(NULL);
      conjunto seleccionados ((char*)"seleccionados");
      conjunto cruzados ((char*)"cruzados");
      conjunto mutados ((char*)"mutados");
      if(debug)
	{
	  cout<<poblacion;
	  getchar();
	}
      seleccionar_conjunto(poblacion, seleccionados, ps);
      if(debug)
	{
	  cout<<seleccionados;
	  getchar();
	}
      cruzar_conjunto(seleccionados, cruzados, cr);
      if(debug)
	{
	  cout<<"POBLACION CRUZADA"<<endl;
	  cout<<cruzados;
	  getchar();
	}
      mutar_conjunto(cruzados, mutados, mr);
      if(debug)
	{
	  cout<<"POBLACION MUTADA"<<endl;
	  cout<<mutados;
	  getchar();
	}
        if(eo==1)
	  {
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
}
