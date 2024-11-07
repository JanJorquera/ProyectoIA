void calculateDistances(vector<vector<double>> coord,
                        vector<vector<double>> t) {
  for (size_t i = 0; i < coord.size(); ++i) {
    for (size_t j = 0; j < coord.size(); ++j) {
      t[i][j] = sqrt(pow(coord[i][0] - coord[j][0], 2.0) +
                     pow(coord[i][1] - coord[j][1], 2.0));
    }
  }
    return;
}

void readFile(string fileName, int &N, int &HP1, int &D, vector<double> T,
              vector<double> S, vector<vector<double>> coord,
              vector<vector<double>> t) {
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
          S.push_back(s);
        }
      } catch (const exception& e) {
        cerr << "Error en formato de archivo" << endl;
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

int main(int argc, char *argv[]) {
  int N, HP1, D;
  vector<double> T;
  vector<double> S;
  vector<vector<double>> t;
  vector<vector<double>> coord;

  readFile(argv[1], N, HP1, D, T, S, coord, t);

  calculateDistances(coord, t);
  return 0;
}