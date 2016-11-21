#include<iostream>
#include<omp.h>
#include <fstream> // ������ � �������


using namespace std;

bool** arr;//�������� ������	
bool** new_arr;//����� ��� ������� ����. ����
int n, m, t;//n,m - ������ �������, t - ���������� ����������� �����
int num_threads = 4;

void copy_array()//����������� ������� new_arr � arr � ������ �����
{
	//����������� (��������) �����
	for(int i = 1; i <= n; i++)
	{
		for(int j = 1; j <= m; j++)
		{
			arr[i][j] = new_arr[i][j];
		}
	}

	//������ � ����� ������� (��� �����)
	for(int i = 1; i <= n; i++)
	{
		arr[i][0] = arr[i][m];
		arr[i][m+1] = arr[i][1];
	}

	//������� � ������ ������� (��� �����)
	for(int i = 1; i <= m; i++)
	{
		arr[0][i] = arr[n][i];
		arr[n+1][i] = arr[1][i];
	}

	//����
	arr[0][0] = arr[n][m];
	arr[0][m+1] = arr[n][1];
	arr[n+1][0] = arr[1][m];
	arr[n+1][m+1] = arr[1][1];
}

void read_data()//���������� ������ �� ����� � ����� ������ �������
{
	int k, x, y;// k - ����������� �����
	cin >> n >> m >> num_threads >> t >> k;
	
	omp_set_dynamic(0);
	omp_set_num_threads( num_threads );
	
	//�������� �������� � ��������� ��������
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
	arr = (bool **) _mm_malloc(((n+2) * sizeof (bool *)), 64);
	new_arr = (bool **) _mm_malloc(((n+2) * sizeof (bool *)), 64);
    for (int i = 0; i < n+2; ++i)
	{
        arr[i] = (bool *) _mm_malloc(((m+2) * sizeof (bool)), 64);
		new_arr[i] = (bool *) _mm_malloc(((m+2) * sizeof (bool)), 64);
		for(int j = 0; j < m+2; j++)
		{
			arr[i][j] = false;
			new_arr[i][j] = false;
		}
	}

	//���������� �����
	for(int i = 0; i < k; i++)
	{
		cin >> x >> y;
		new_arr[x][y] = true;
	}

	//��������� �����������
	copy_array();
}

/*void write_array() //��� ��������
{
	system("cls");
	for(int i = 0; i < n+2; i++)
	{
		for(int j = 0; j < m+2; j++)
		{
			if(arr[i][j])
				cout << "0 ";
			else
				cout << ". ";
		}
		cout << endl;
	}
}*/

int main()
{
	freopen("cells_data.txt","r",stdin);
	read_data();
	double finish_time = 0;
	double start_time = omp_get_wtime( );
	//write_array();
	#pragma omp parallel
	{
		for(int counter = 0; counter < t; counter++)
		{	
			#pragma omp for schedule(static)
			for(int i = 1; i <= n; i++)
			{
				#pragma omp simd
				for(int j = 1; j <= m; j++)
				{
					int k = 0;
					if(arr[i][j+1]) k++;
					if(arr[i+1][j]) k++;
					if(arr[i][j-1]) k++;
					if(arr[i-1][j]) k++;
					if(arr[i+1][j+1]) k++;
					if(arr[i-1][j+1]) k++;
					if(arr[i+1][j-1]) k++;
					if(arr[i-1][j-1]) k++;
				
					if(arr[i][j] && k != 3 && k != 2)//�����������
						new_arr[i][j] = false;
					if(!arr[i][j] && k == 3)//��������
						new_arr[i][j] = true;
					//� ������ ������� ������ �� ������
				}
			}
			//�������� ��� ���� �������� � �������� ������
			//����������� (��������) �����
			#pragma omp for schedule(static)
			for(int i = 1; i <= n; i++)
			{
				#pragma omp simd
				for(int j = 1; j <= m; j++)
				{
					arr[i][j] = new_arr[i][j];
				}
			}

			//������ � ����� ������� (��� �����)
			#pragma omp for schedule(static) 
			for(int i = 1; i <= n; i++)
			{
				arr[i][0] = arr[i][m];
				arr[i][m+1] = arr[i][1];
			}

			//������� � ������ ������� (��� �����)
			#pragma omp for schedule(static) 
			for(int i = 1; i <= m; i++)
			{
				arr[0][i] = arr[n][i];
				arr[n+1][i] = arr[1][i];
			}

			//����
			arr[0][0] = arr[n][m];
			arr[0][m+1] = arr[n][1];
			arr[n+1][0] = arr[1][m];
			arr[n+1][m+1] = arr[1][1];
		//	write_array();
		}
	}
	finish_time = omp_get_wtime( );
	ofstream fout("alg_1_runtime.txt", ios_base::app);
	fout << " " << finish_time - start_time;
	fout.close();
	//write_array();

	//������������ ������
    for (int i = 0; i < n+2; ++i)
	{
         _mm_free(arr[i]);
		 _mm_free(new_arr[i]);
	}
	_mm_free(arr);
	_mm_free(new_arr);
	return 0;
}