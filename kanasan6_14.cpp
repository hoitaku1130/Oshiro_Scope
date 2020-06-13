// kanasan6_5.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void print_data(const char* filename,float *in,int Length) {
	FILE *fp;
	fp = fopen(filename,"w");
	for (int i = 0; i < Length; i++) {
		fprintf(fp, "%f\n", *(in + i));
	}
	fclose(fp);
}

void print_data_int(const char* filename, int *in, int Length) {
	FILE *fp;
	fp = fopen(filename, "w");
	for (int i = 0; i < Length; i++) {
		fprintf(fp, "%d\n", *(in + i));
	}
	fclose(fp);
}

#if 0
void smoothing_moving_average(int smoothing_factor, int *in, int Length, long long *out) {	//smoothing_factorは何点分の移動平均を作るか
	int i;
	if (Length < smoothing_factor) return;
	out[0] = (long long)in[0];
	if (smoothing_factor % 2 == 0) {
		for (i = 1; i < smoothing_factor / 2 - 1; i++) {
			out[i] = (long long)in[i] + (long long)out[i - 1];
		}
		for (int j = 0; j <= smoothing_factor / 2; j++) {
			if (j == 0) {
				out[i] = out[i - 1] + (long long)in[i];
			}
			else {
				out[i] += (long long)in[i + j];
			}
		}
		i++;
		for (; i < (Length - smoothing_factor / 2); i++) {
			out[i] = out[i - 1] - (long long)in[i - smoothing_factor / 2] + (long long)in[i + smoothing_factor / 2];
		}
		for (; i < Length; i++) {
			out[i] = out[i - 1];	//どうでもいいので一応前の値コピー
		}
	}
	else {
		for (i = 1; i < smoothing_factor / 2; i++) {
			out[i] = (long long)in[i] + (long long)out[i - 1];
		}
		for (int j = 0; j <= smoothing_factor / 2; j++) {
			if (j == 0) {
				out[i] = out[i - 1] + (long long)in[i];
			}
			else {
				out[i] += (long long)in[i + j];
			}
		}
		i++;
		for (; i < (Length - smoothing_factor / 2); i++) {
			out[i] = out[i - 1] - (long long)in[i - smoothing_factor / 2 - 1] + (long long)in[i + smoothing_factor / 2];
		}
		for (; i < Length; i++) {
			out[i] = out[i - 1];	//どうでもいいので一応前の値コピー
		}
	}

}

int bit_shift_negative(int in,int shift_num) {
	if ( in < 0) {
		return ~((~in)>>shift_num);			//通常＋１するが今回はオーバーフローしてしまうことを想定しいれない（-21... + 1）
	}
	else {
		return in>>shift_num;
	}
}

void smoothing_moving_average_with_bitShift(int smoothing_factor, int *in, int Length, int *out) {
	int shift_num = smoothing_factor / 2 + 1;
	int i;
	if (Length < smoothing_factor) return;
	
	out[0] = in[0]>>(smoothing_factor/2-1);
	out[0] = bit_shift_negative(in[0],shift_num);
	if (smoothing_factor % 2 == 0) {
			for (i = 1; i < smoothing_factor / 2 - 1; i++) {
				out[i] = bit_shift_negative(in[i],(shift_num - 1)) + out[i - 1];
			}
			for (int j = 0; j <= smoothing_factor / 2; j++) {		//中心から-部分の足し算
				if (j == 0) {
					out[i] = bit_shift_negative(out[i - 1], shift_num) + bit_shift_negative(in[i], shift_num);
				}
				else {
					out[i] += bit_shift_negative(in[i + j], shift_num);	//中心から+部分の足し算
				}
			}
			i++;
			for (; i < (Length - smoothing_factor / 2); i++) {
				out[i] = (out[i - 1] - in[i - smoothing_factor / 2]) + in[i + smoothing_factor / 2];
			}
			for (; i < Length; i++) {
				out[i] = out[i - 1];	//どうでもいいので一応前の値コピー
			}
		}
	else {
			for (i = 1; i < smoothing_factor / 2; i++) {
				out[i] = bit_shift_negative(in[i], (shift_num - 1)) + out[i - 1];
			}
			for (int j = 0; j <= smoothing_factor / 2; j++) {
				if (j == 0) {
					out[i] = bit_shift_negative(out[i - 1], shift_num) + bit_shift_negative(in[i], shift_num);
				}
				else {
					out[i] += bit_shift_negative(in[i + j], shift_num);
				}
			}
			i++;
			for (; i < (Length - smoothing_factor / 2); i++) {
				out[i] = (out[i - 1] - in[i - smoothing_factor / 2 - 1]) + in[i + smoothing_factor / 2];
			}
			for (; i < Length; i++) {
				out[i] = out[i - 1];	//どうでもいいので一応前の値コピー
			}
	}
}
#endif

void smoothing_moving_average_with_float(int smoothing_factor,int *in,int Length,int *out) {	//奇数のみ
	int i;
	float sum = 0;
	int j;
	if (Length < smoothing_factor) return;
	if (smoothing_factor%2 == 1) {
		i = smoothing_factor / 2 ;
		for (j = -smoothing_factor/2; j < smoothing_factor/2;j++) {
			sum += in[j + i];
		}
		out[i] = sum / smoothing_factor;
		for (j = 0; j < i;j++) {
			out[j] = out[i];
		}
		i++;
		for (; i < Length - (smoothing_factor/2);i++) {
			sum = ( sum - (float)in[i - smoothing_factor/2-1] + (float)in[i + smoothing_factor/2]);
			out[i] = (int)sum / smoothing_factor;
			//out[i] = (int) ( ((float)out[i - 1]*smoothing_factor - (float)in[i - smoothing_factor / 2 - 1] + (float)in[i + smoothing_factor / 2])/smoothing_factor);
			//out[i] = out[i-1];
			/*for (j = -smoothing_factor / 2; j < smoothing_factor/2; j++) {
				sum += in[j + i];
			}
			out[i] = sum / (smoothing_factor);
			sum = 0;*/
		}
		for (; i < Length;i++) {
			out[i] = out[i-1];
		}
	}
	else {
		
	}



}

int negative_peak_search_core(int *in,int *out,int i,int len,int *j,int *in_x,int line) {
	int a, b, c;
	a = in[i];
	b = in[i + len];
	c = in[i + 2 * len];

	if (b > line) {
		return 1;
	}

	if (a > b && b < c) {
		out[*j] = in_x[i + 1];
		(*j)++;
		return 2;
	}
	else if (a == b && b < c) {
		out[*j] = in_x[i + 1];
		(*j)++;
		return 2;
	}
	else if (a < b && b < c) {
		return 2;
	}
	else {
		return 1;
	}
}

int negative_find_peaks_for_int(int *in, int Length, int *out, int len,int *in_x,int line) {		//特にエラーハンドリング等はしていないが、outがLengthを超えることはない
	int i = 0;
	int j = 0;
	int next;
	for (; i + 2 * len < Length;) {
		next = negative_peak_search_core(in, out, i, len, &j,in_x,line);
		i += next * len;
	}
	return j;
}


int peak_search_core(int *in, int *out, int i, int len, int *j) {		//iはピークサーチの走査用、jはoutに書き込む用
	int a, b, c;
	a = in[i];
	b = in[i + len];
	c = in[i + 2 * len];
	if (a < b && b < c) {
		return 1;
	}
	else if (a < b && b == c) {
		return 1;
	}
	else if (a < b && b > c) {
		out[*j] = i + 1;
		(*j)++;
		return 2;
	}
	else if (a == b && b < c) {
		return 1;
	}
	else if (a == b && b == c) {
		return 1;
	}
	else if (a == b && b > c) {
		out[*j] = i + 1;
		(*j)++;
		return 2;
	}
	else if (a > b && b < c) {
		return 1;
	}
	else if (a > b && b == c) {
		return 1;
	}
	else if (a > b && b > c) {
		return 2;
	}
	else{
		return 1;
	}		
}


int find_peaks_for_int(int *in, int Length, int *out,int len) {		//特にエラーハンドリング等はしていないが、outがLengthを超えることはない
	int i = 0;
	int j = 0;
	int next;
	for (; i + 2*len < Length;){
		next = peak_search_core(in, out, i, len, &j);
		i += next * len;
	}
	return j;
}

int peak_search_core_float(float *in, int *out, int i, int len, int *j,float line) {		//iはピークサーチの走査用、jはoutに書き込む用
	float a, b, c;
	a = in[i];
	b = in[i + len];
	c = in[i + 2 * len];
	if (a < b && b < c) {
		return 1;
	}
	else if (a < b && b == c) {
		return 1;
	}
	else if (a < b && b > c) {
		if (b < line) {
			return 2;
		}
		out[*j] = i + 1;
		(*j)++;
		return 2;
	}
	else if (a == b && b < c) {
		return 1;
	}
	else if (a == b && b == c) {
		return 1;
	}
	else if (a == b && b > c) {
		if (b < line) {
			return 2;
		}
		out[*j] = i + 1;
		(*j)++;
		return 2;
	}
	else if (a > b && b < c) {
		return 1;
	}
	else if (a > b && b == c) {
		return 1;
	}
	else if (a > b && b > c) {
		return 2;
	}
	else {
		return 1;
	}
}


//こちらは実数用
//lineでしきい値を指定する
int find_peaks_for_float(float *in, int Length, int *out, int len,float line) {		//特にエラーハンドリング等はしていないが、outがLengthを超えることはない
	int i = 0;
	int j = 0;
	int next;
	for (; i + 2 * len < Length;) {
		next = peak_search_core_float(in, out, i, len, &j,line);
		i += next * len;
	}
	return j;
}

//inはフリンジピーク
//Lengthはinの長さ
//outは節となるピーク位置保存用
//out_lenは見つけた節ピークの数
//smoothing_factor_strong 比較的大きい値(移動平均に使うデータ数)
//smoothing_factor_weak は比較的小さい値
//delta_x 3点ピークサーチの点と点の距離
//line は smoothing_strong/smoothing_weak を　divisionした際のしきい値

//0以外エラー

int peaks_search_from_division_with_different_smoothing(int *in,int Length,int *out,int *out_len,int smoothing_factor_strong,int smoothing_factor_weak,int delta_x,float line) {	//6月5日の①②③まで
	int i;
	//strong, weak, divisionの３つ作成 
	int *strong, *weak;
	float *division;
	strong = (int *) malloc(sizeof(int) * Length * 3);
	if (strong == NULL) {
		return -1;
	}
	weak = strong + Length;
	division = (float*) (strong + Length * 2);
	smoothing_moving_average_with_float(smoothing_factor_strong,in,Length,strong);
	smoothing_moving_average_with_float(smoothing_factor_weak,in,Length,weak);
	for (i = 0; i < Length;i++) {
		if (weak[i] == 0) {
			division[i] = 1;
		}
		else {
			division[i] = (float)strong[i] / weak[i];			//② (ゼロで割る可能性もあるので対策必要  エラーハンドリング必須)
		}													
	}
	print_data("division.txt",division,Length);
	
	*out_len = find_peaks_for_float(division, Length, out, delta_x, line);		//③
	
	free(strong);
	return 0;
}

int peak_search_on_smoothing_data(int *in,int Length,int *out,int *out_len,int smoothing_factor) {
	int *data = (int *)malloc(sizeof(int) * Length);
	if (data == NULL) {
		printf("error\n");
		return -1;
	}
	smoothing_moving_average_with_float(smoothing_factor,in,Length,data);
	*out_len = find_peaks_for_int(data,Length,out,1);
	print_data_int("output_smooth.txt",data,Length);
	free(data);
	return 0;
}

// ------------------------- //6月14日// --------------------------

int Dispersion_core(int *in,int start_i,int end_i,int len) {		//len = (end_i - start_i + 1)
	int i;
	double sum = 0, sum2 = 0;
	for (i = start_i; i <= end_i;i++) {
		sum += in[i];
		sum2 += pow(in[i],2);
		
	}
	return sqrt(sum2/len - pow(sum/len,2));
}

int Dispersion(int *in,int *out_x,int *out_y,int Length,int ADP) {		//標準偏差を求めデータ収納　x, yデータで出力
	int i;
	int j = 0;
	for (i = 0; i < Length - ADP;i+=ADP) {
		out_y[j] = Dispersion_core(in,i,i+ADP,ADP+1);
		out_x[j] = i + ADP / 2;
		j++;
	}
	return j;
}

int average_of_distance_between_peaks(int *in,int Length) {
	int i;
	double sum = 0;
	for (i = 1; i < Length;i++) {
		sum += in[i] - in[i-1];		//ピーク間隔
	}
	return (int)(sum / Length + 0.5);		//四捨五入
}

int new_approach(int *in, int Length, int *out, int *out_len, int smoothing_factor, int line) {
	int *data = (int *)malloc(sizeof(int) * Length * 5);
	int *data_of_dispersion_x = data + Length;
	int *data_of_dispersion_y = data + Length * 2;
	int *data_of_smth = data + Length * 3;
	int *data_of_negative = data + Length * 4;
	if (data == NULL) {
		printf("Error\n");
		return -1;
	}
	int peak_length = find_peaks_for_int(in,Length,data,1);		//①
	int ADP = average_of_distance_between_peaks(data,peak_length);		//②
	int dispersion_len = Dispersion(in,data_of_dispersion_x,data_of_dispersion_y,Length,ADP);	//③
	smoothing_moving_average_with_float(smoothing_factor,data_of_dispersion_y,dispersion_len,data_of_smth);		//③続き
	int negative_len = negative_find_peaks_for_int(data_of_smth,dispersion_len,data_of_negative,3,data_of_dispersion_x,line);

	printf("%d\n",ADP);
	print_data_int("output_first_3points_peak_search.txt",data,peak_length);
	print_data_int("dispersion_x.txt",data_of_dispersion_x,dispersion_len);
	print_data_int("dispersion_y.txt",data_of_dispersion_y,dispersion_len);
	print_data_int("smoothing.txt",data_of_smth,dispersion_len);
	print_data_int("negative.txt",data_of_negative,negative_len);
	free(data);
}

#define N_LEN 200000

int main(int argc,char** argv)
{
	FILE *fp;
	int *data;
	int *in;
	int out_len = 0;
	fp = fopen("data.txt", "r");
	if (fp == NULL) {
		printf("error of opening file\n");
		return -1;
	}
	data = (int *)malloc(N_LEN * sizeof(int));
	in = (int*)malloc(N_LEN * sizeof(int));
	double tmp;
	for (int i = 0; i < N_LEN; i++) {
		fscanf(fp, "%lf", &tmp);
		in[i] = (int)tmp;
	}
	fclose(fp);
	fp = fopen("output.txt", "w");
	//smoothing_moving_average_with_float(atoi(argv[1]), in, N_LEN, data);
	//int res = peaks_search_from_division_with_different_smoothing(in, N_LEN, data, &out_len, 1001, 51, 10, (float)1.2);
	//int res = peak_search_on_smoothing_data(in,N_LEN,data,&out_len,61);

	int res = new_approach(in,N_LEN,data,&out_len,101,500);

	//printf("%d\n",res);
	for (int i = 0; i < out_len; i++) {
		fprintf(fp, "%d %d\n", *(data + i),1);
	}
	fclose(fp);
	free(in);
	free(data);
	puts("success");
	//printf("%lld\n",c);
	//printf("%d\n",bit_shift_negative(-300034,2));
	//printf("%d\n",(~0x80000000 + 1));
	//printf("%d\n",(1 << 31) & (-pow(2,31)));
	getchar();
	return 0;
}

