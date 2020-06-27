#include <math.h>
#include "Make_Interferogram.h"

int MakeInterferogram::Make_Local_STDDEV_core(int *in,int start_i,int end_i,int len){
    int i;
    double sum = 0, sum2 = 0;
    for (i = start_i; i <= end_i;i++) {
        sum += in[i];
        sum2 += pow(in[i],2);
    }
    return (int)(sqrt(sum2/len - pow(sum/len,2))+0.5);

};

int MakeInterferogram::Make_Local_STDDEV(int *in,int *out_x,int *out_y,int Length,int ADP){
    int i;
    int j = 0;
    for (i = 0; i < Length - ADP;i+=ADP) {
        out_y[j] = this->Make_Local_STDDEV_core(in,i,i+ADP,ADP+1);
        out_x[j] = i + ADP / 2;
        j++;
    }
    return j;
};

int MakeInterferogram::Make_Moving_Average(int *in,int Length,int *out,int smoothing_factor,int *median){
    int i;
    double _sum = 0;
    int max_index = smoothing_factor/2,min_index = smoothing_factor/2;
    int j;
    if(Length < smoothing_factor){
        return -1;
    }
    if((smoothing_factor%2) == 1){
        i = smoothing_factor / 2;
        for(j = -smoothing_factor/2; j < smoothing_factor/2;j++){
            _sum += in[j+i];
        }
        out[i] = (int) (_sum/smoothing_factor);
        for(j = 0;j<i;j++){
            out[j] = out[i];
        }
        i++;
        for(;i<Length - (smoothing_factor/2);i++){
            _sum = ( _sum - (double) in[i-smoothing_factor/2-1] + (double) in[i+smoothing_factor/2]);
            out[i] = (int) (_sum/smoothing_factor + 0.5);
            if(out[i] > out[max_index]){    //最大値最小値を求める用
                max_index = i;
            }
            if(out[i] < out[min_index]){
                min_index = i;
            }
        }
        for(;i<Length;i++){
            out[i] = out[i-1];
        }

    }else{  //even
        i = smoothing_factor / 2;
        for(j = -smoothing_factor/2;j<smoothing_factor/2-1;j++){
            _sum += in[j+i];
        }
        out[i] = (int) (_sum/smoothing_factor);
        for(j=0;j<i;j++){
            out[j] = out[i];
        }
        i++;
        for(;i<Length - (smoothing_factor/2-1);i++){
            _sum = (_sum - (double) in[i-smoothing_factor/2-1] + (double) in[i+smoothing_factor/2-1]);
            out[i] = (int) (_sum/smoothing_factor + 0.5);
            if(out[i] > out[max_index]){
                max_index = i;
            }
            if(out[i] < out[min_index]){
                min_index = i;
            }
        }
        for(;i<Length;i++){
            out[i] = out[i-1];
        }
    }
    *median = (out[max_index] + out[min_index])/2;
    return Length;
};

int MakeInterferogram::ThreePoint_Negative_Peak_Search_core(int *in,int *out,int i,int delta,int *j,int line){
    int a,b,c;
    a = in[i];
    b = in[i + delta];
    c = in[i + delta * 2];
    if(a > b && b < c){
        if(b < line){
            out[*j] = i + delta;
            (*j)++;
            return 2;
        }else{
            return 2;
        }
    }else if(a == b && b < c){
        if(b < line){
            out[*j] = i + delta;
            (*j)++;
            return 2;
        }else{
            return 2;
        }
    }else if(a < b && b < c){
        return 2;
    }else{
        return 1;
    }
};

int MakeInterferogram::ThreePoint_Negative_Peak_Search(int *in,int Length,int *out,int delta,int line){
    int i = 0;
    int j = 0;
    int next;
    for(;i+2*delta<Length;){
        next = this->ThreePoint_Negative_Peak_Search_core(in,out,i,delta,&j,line);
        i += next * delta;
    }
    return j;
};

int MakeInterferogram::ThreePoint_Positive_Peak_Search_core_for_Zero_Burst(int *in,int *out,int i,int delta){
    int a,b,c;
    a = in[i];
    b = in[i + delta];
    c = in[i + delta * 2];
    if(a < b && b > c){
        *out = i + delta;
        return 2;
    }else if(a == b && b > c){
        *out = i + delta;
        return 2;
    }else if(a > b && b > c){
        return 2;
    }else{
        return 1;
    }
};

int MakeInterferogram::Find_Zero_Burst(int *in_sample,int Length,int *section,int sec_len,int *end_i){
    int start_index,end_index;
    int subtract1,subtract2;
    if(sec_len < 2){        //そもそも節点が2つ以上ない場合はエラー判定
        return -1;
    }
    if(sec_len < 3){
        start_index = section[0];
        end_index = section[1];
    }else{
        subtract1 = section[1] - section[0];
        subtract2 = section[2] - section[1];
        if(subtract1 > subtract2){
            start_index = section[0];
            end_index = section[1];
        }else{
            start_index = section[1];
            end_index = section[2];
        }
    }
    *end_i = end_index; //節の終わりを外部に知らせる
    int i,next;
    int max=0,temporary=0;
    for(i=0;i+2 < Length;){
        next = this->ThreePoint_Positive_Peak_Search_core_for_Zero_Burst(in_sample,&temporary,i,1);
        if(next == 2){
            if(in_sample[max] < in_sample[temporary]){
                max = temporary;
            }
        }
        i += next;
    }
    return max;
};

int MakeInterferogram::ThreePoint_Positive_Peak_Search_core_for_Make_Interfero(int *in,int *sample,int *out,int i,int delta,int *j){
    int a,b,c;
    a = in[i];
    b = in[i + delta];
    c = in[i + delta * 2];
    if(a < b && b > c){
        out[*j] = sample[i + delta];
        (*j)++;
        return 2;
    }else if(a == b && b > c){
        out[*j] = sample[i + delta];
        (*j)++;
        return 2;
    }else if(a > b && b > c){
        return 2;
    }else{
        return 1;
    }
};

int MakeInterferogram::ThreePoint_Positive_Peak_Search_for_Make_Interfero(int *in_sample,int *in_fringe,int Length,int *out,int delta,int limited){
    int i = 0;
    int j = 0;
    int next;
    for(;i+2<Length;){
        next = this->ThreePoint_Positive_Peak_Search_core_for_Make_Interfero(in_fringe,in_sample,out,i,delta,&j);
        i += next * delta;
        if(j > limited){
            return j;
        }
    }
    return j;
};

int MakeInterferogram::Make_Interfero(int *in_sample,int *in_fringe,int end_i,int *out,int Zero_burst_index,int limited){
    if(Zero_burst_index > end_i){
        return -1;
    }
    return this->ThreePoint_Positive_Peak_Search_for_Make_Interfero(in_sample + Zero_burst_index,in_fringe + Zero_burst_index,end_i - Zero_burst_index,out,1,limited);
};

