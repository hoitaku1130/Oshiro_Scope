#ifndef MAKE_INTERFEROGRAM_H
#define MAKE_INTERFEROGRAM_H

class MakeInterferogram{
public:
    int Make_Local_STDDEV(int *in,int *out_x,int *out_y,int Length,int ADP);    //inは入力データ基本生データ, Lengthはinの入力データのサイズ,
                                                                            //ADP は正常なフリンジパターンが何点でできているかを入れる。
                                                                        //return はoutの個数

    int Make_Moving_Average(int *in,int Length,int *out,int smoothing_factor,int *median); //Lengthはin(入力データ(ローカル標準偏差))の個数
                                                                               //smoothing_factorは何個の移動平均をするか
                                                                               //returnはoutの個数 マイナスならエラー
                                                                               //medianは次の関数で使うようのしきい値を出力

    int ThreePoint_Negative_Peak_Search(int *in,int Length,int *out,int delta,int line);  //inは入力データ, deltaは何点間隔で比較するか
                                                                                        //lineはしきい値

    int Find_Zero_Burst(int *in_sample,int Length,int *section,int sec_len,int *end_i);  //return はゼロ点バーストインデックスを返すただしin_sampleを0とした
                                                                                            //sectionは節点データ、sec_lenはその長さ
                                                                                          //end_iは節の終わりを出力する

    int Make_Interfero(int *in_sample,int *in_fringe,int end_i,int *out,int Zero_burst_index,int limited);//returnはoutの長さ, end_iは節点の終了地点
                                                                                                    //limitedはユーザーが分解能を指定してきた場合いれる

private:
    int Make_Local_STDDEV_core(int *in,int start_i,int end_i,int len); //Make_Local_STDDEV用関数
    int ThreePoint_Negative_Peak_Search_core(int *in,int *out,int i,int delta,int *j,int line);  //ThreePoint_Negative_Peak_Search用関数
    int ThreePoint_Positive_Peak_Search_core_for_Zero_Burst(int *in,int *out,int i,int delta);   //Find_Zero_Burst関数用
    int ThreePoint_Positive_Peak_Search_core_for_Make_Interfero(int *in,int *sample,int *out,int i,int delta,int *j);
    int ThreePoint_Positive_Peak_Search_for_Make_Interfero(int *in_sample,int *in_fringe,int Length,int *out,int delta,int limited); //Make_Interfero関数で使う
};

#endif // MAKE_INTERFEROGRAM_H
