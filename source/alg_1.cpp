#include<iostream>
#include<omp.h>
#include <fstream> // работа с файлами


using namespace std;

bool* arr;//основной массив	
bool* new_arr;//копия для расчёта след. шага
int n, m, t;//n,m - размер массива, t - переменная колличества шагов
int M, N;
int num_threads = 4;

void copy_array()//копирование массива new_arr в arr с учётом рамки
{
	//центральная (основная) часть
	for(int i = 1; i <= n; i++)
	{
		for(int j = 1; j <= m; j++)
		{
			arr[i*M + j] = new_arr[i*M + j];
		}
	}

	//правая и левая стороны (без углов)
	for(int i = 1; i <= n; i++)
	{
		arr[i*M] = arr[i*M + m];
		arr[i*M + m+1] = arr[i*M + 1];
	}

	//верхняя и нижняя стороны (без углов)
	for(int j = 1; j <= m; j++)
	{
		arr[j] = arr[n*M + j];
		arr[(n+1)*M + j] = arr[M + j];
	}

	//углы
	arr[0] = arr[n*M + m];
	arr[m+1] = arr[n*M + 1];
	arr[(n+1)*M] = arr[1*M + m];
	arr[(n+1)*M + m+1] = arr[M + 1];
}

void read_data()//считывавем данные из файла и задаём разеры массива
{
	ifstream fin("cells_data.txt", ios_base::in);

	int k, x, y;// k - колличество точек
	fin >> n >> m >> num_threads >> t >> k;
	
	omp_set_dynamic(0);
	omp_set_num_threads( num_threads );
	
	//создание массивов и начальные значения
	/*arr = new bool*[n+2];
	new_arr = new bool*[n+2];
	for(int i = 0; i < n+2; i++)
	{
		arr[i] = new bool[m+2];
		new_arr[i] = new bool[m+2];
		for(int j = 0; j < m+2; j++)
		{
			arr[i][j] = false;
			new_arr[i][j] = false;
		}
	}*/
	N = n+2;
	M = m+2;
	new_arr = (bool *) _mm_malloc(((N)* (M) * sizeof (bool )), 64);
	arr = (bool *) _mm_malloc(((N)* (M) * sizeof (bool )), 64);

    for (int i = 0; i < n+2; ++i)
	{
		for(int j = 0; j < m+2; j++)
		{
			arr[i*M + j] = false;
			new_arr[i*M + j] = false;
		}
	}

	
	//считывание точек
	for(int i = 0; i < k; i++)
	{
		fin >> x >> y;
		new_arr[x*(m+2) + y] = true;
	}

	fin.close();

	//запускаем копирование
	copy_array();
}

/*void write_array() //для проверки
{
	system("cls");
	for(int i = 0; i < n+2; i++)
	{
		for(int j = 0; j < m+2; j++)
		{
			if(arr[i*M + j])
				cout << "0 ";
			else
				cout << ". ";
		}
		cout << endl;
	}
}*/

int main()
{
	read_data();
	double finish_time = 0;
	double start_time = omp_get_wtime( );
	//write_array();
	for(int counter = 0; counter < t; counter++)
	{		
		#pragma omp parallel for schedule(static)
		for(int i = 0; i < n; i++)
		{
			#pragma omp simd
			for(int j = 0; j < m; j++)
			{
				//ведём расчёт на ячейку i+1 j+1
				int CurI = i*M+j; 
				int k = arr[CurI] + arr[CurI+1] + arr[CurI + 2] + arr[CurI + M ] + arr[CurI + M + 2 ] + arr[CurI + M + M]  
					+ arr[CurI + M + M + 1] + arr[CurI + M + M + 2] ;				
				//k += arr[i+ki][j+kj];	
				CurI = CurI + M + 1; 
				if(arr[CurI] && k != 3 && k != 2)//умертвление
				{
					new_arr[CurI] = false;
				}
				if(!arr[CurI] && k == 3)//создание
				{
					new_arr[CurI] = true;
				}
				//в других случаях ничего не меняем
			}
		}
		//копируем все наши действия в основной массив
		//центральная (основная) часть
		#pragma omp parallel for schedule(static)
		for(int i = 1; i <= N*M; i++)
		{
			arr[i] = new_arr[i];
		}

		//правая и левая стороны (без углов)
		#pragma omp parallel for schedule(static)
		for(int i = 1; i <= n; i++)
		{
			arr[i*M] = arr[i*M + m];
			arr[i*M + m+1] = arr[i*M + 1];
		}

		//верхняя и нижняя стороны (без углов)
		#pragma omp parallel for schedule(static)
		for(int j = 1; j <= m; j++)
		{
			arr[j] = arr[n*M + j];
			arr[(n+1)*M + j] = arr[M + j];
		}
		//углы
		arr[0] = arr[n*M + m];
		arr[m+1] = arr[n*M + 1];
		arr[(n+1)*M] = arr[1*M + m];
		arr[(n+1)*M + m+1] = arr[M + 1];
		//write_array();
	}
	finish_time = omp_get_wtime( );
	ofstream fout("alg_1_runtime.txt", ios_base::app);
	fout << " " << finish_time - start_time;
	fout.close();
	//write_array();

	return 0;
}