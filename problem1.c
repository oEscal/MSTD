#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <strings.h>
#include <string.h>
#include "elipsed_time.h"

const unsigned short MIN_N = 1,                                                           //minimum value of n
                     MAX_N = 32;                                                          //maximum value that n can have in this implementation
FILE *data_file;                                                                          //file where is stored all the data collected while this program runs, if user wants to save it


//function given by AED professor and returns the number of 1 bits of given mask (argument)
int population_count(unsigned long mask){
   int count;
   
   for(count = 0;mask != 0ul;count++)
      mask &= mask - 1ul;
   return count;
}

/* Function to calc the values stored in a given set (by index)
 *
 * Arguments:
 *    -> unsigned long set: set where are the values to extract
 *    -> unsgined short n: actual n (that is the max value in set)
 *    -> long *set_values: address of the array where is to store the values
 * Return:
 *    -> number of values in set
 * Example:
 *    -> the set 1101 (n = 4) corresponds to values {4, 3, 1}. So, this set has 3 values
 */
unsigned int setValues(unsigned long set, unsigned short n, long *set_values){
   unsigned int index = 0;

   for(int value = 1; set != 0; value++){
      if((set & 1ul) == 1ul)
         set_values[index++] = value;

      set = set >> 1ul;
   }

   return index;
}

/* Function to calc the sum and diff sets of A (slowest version)
 *
 * Arguments:
 *    -> unsigned long set: set where are the values to sum and subtract
 *    -> unsigned short n: actual n (that is the max value in set)
 *    -> unsigned long *result: where are stored the results of sum and diff sets of A (result[0] = result_sum; result[1] = result_dif)
 * 
 * Attention:
 *    -> the unsigned long *result argument must be allocated before calling this function
 */
void normalBruteForce(unsigned long set, int n, unsigned long *result){
   unsigned long result_sum = 0,
                 result_dif = 0;

   long set_values[n];
   unsigned int size = setValues(set, n, set_values);

   for(int index1 = 0; index1 < size; index1++)
      for(int index2 = index1; index2 < size; index2++){
         result_sum |= (unsigned long)pow(2, set_values[index1] + set_values[index2] - 1);
         result_dif |= (unsigned long)pow(2, set_values[index2] - set_values[index1]);
      }

   result[0] = result_sum;
   result[1] = result_dif;

}

/* Function to calc the sum and diff sets of A (fastest version)
 *
 * Arguments:
 *    -> unsigned long set: set where are the values to sum and subtract
 *    -> unsigned short n: actual n (that is the max value in set)
 *    -> unsigned long *result: where are stored the results of sum and diff sets of A (result[0] = result_sum; result[1] = result_dif)
 * 
 * Attention:
 *    -> the unsigned long *result argument must be allocated before calling this function
 *    -> the explanation for this method is in the report attached with this document. However, due to my elementary english knowledge, 
 *       it is in portuguese. Nevertheless, if you feel comfortable with binary logic, you will find no problems to understand this piece of code, I think
 */
void fastestBruteForce(unsigned long set, unsigned short n, unsigned long *result){

   unsigned long result_sum = 0,
                 result_dif = 0;

   unsigned long set_copy = set;
   for(int value = 1; set_copy != 0; value++){
      if((set_copy & 1ul) == 1ul){
         result_sum |= set << value;
         result_dif |= set >> (value - 1);
      }
      set_copy >>= 1ul;
   }

   result[0] = result_sum;
   result[1] = result_dif;

}

/* Function to calc, essentially, the number of MSTDs for a given n
 *
 * Arguments:
 *    -> unsigned short n: actual n (that is the max value in set)
 *    -> short save_data: boolean value (value >0 if the user wants to save the data to a file; <=0 if user doesn't wants to save the data)
 *    -> unsigned short version: variable with the version that user wants to run (0 if normal brute force; 1 if fastest brute force)
 * Return:
 *    -> number of MSTDs for a giver n
 */
unsigned long mstd(unsigned short n, short save_data, unsigned short version){

   unsigned long A_set,                                                                   //where is stored the values of A in the next while loop (the values of A corresponds to the indexes of 1 bits of A_set plus 1)
                 max_value = pow(2, n - 1);                                               //this is the max value that A_set can reach, because it can just have n bits with the value of 1

   unsigned int number_of_mstd_sets = 0;                                                  //number of MSTD sets for given n
   unsigned short biggest_value_sum = 0,                                                  //biggest value of |A + A| for given n
                  biggest_value_dif = 0,                                                  //smallest value of |A + A| for given n
                  smallest_value_sum = __SHRT_MAX__*2 + 1,                                //biggest value of |A - A| for given n
                  smallest_value_dif = __SHRT_MAX__*2 + 1;                                //smallest value of |A - A| for given n
   short biggest_value_sum_and_dif = 0,                                                   //biggest value of |A + A| - |A - A| for given n
         smallest_value_sum_and_dif = __SHRT_MAX__;                                       //smallest value of |A + A| - |A - A| for given n
   
   unsigned short sum,                                                                    //current value of |A + A|
                  dif;                                                                    //current value of |A - A|
   short sum_and_dif;                                                                     //current value of |A + A| - |A - A|

   unsigned long result_sum,                                                              //variable where is stored all the A + A values (each index of 1 bit corresponds to one of the sum's value)
                 result_dif,                                                              //variable where is stored all the positive (and 0) A - A values (each index of 1 bit corresponds to one of the sum's value)
                 *result = malloc(2*sizeof(result_sum));                                  //variable where are stored the current values of result_sum (index 0) and result_dif (index 1) (this array is obtained through the normalBruteForce() or fastestBruteForce() functions)
   

   for(int n1 = 0; n1 < max_value; n1++){
      A_set = max_value + n1;                                                             //to cover all the possible A set values

      if(version == 0)
         normalBruteForce(A_set, n, result);
      else
         if(version == 1)
            fastestBruteForce(A_set, n, result);
      result_sum = result[0];                                                             //update the data struct where is saved all the A set sums
      result_dif = result[1];                                                             //update the data struct where is saved all the positive (and zero) A set differences

      sum = population_count(result_sum);
      dif = 2*population_count(result_dif) - 1;                                           //the reason for such value is that in result_dif just are placed the positive (and zero) results of A - A. As for any two different numbers there are two possible results for the difference, beig one mymmetrical to the other (-15 and 15 for exemple), the number of differences in the A - A set is equal to 2*(number of positive results of all differences) - 1 (-1 because "there are not two 0")
      sum_and_dif = sum - dif;

      //update the number of MSTD sets and the biggest and the smallest values of |A + A|, |A - A| and |A + A| - |A - A|
      if(sum > dif)
         number_of_mstd_sets++;
      if(sum > biggest_value_sum)
         biggest_value_sum = sum;
      if(sum < smallest_value_sum)
         smallest_value_sum = sum;
      if(dif > biggest_value_dif)
         biggest_value_dif = dif;
      if(dif < smallest_value_dif)
         smallest_value_dif = dif;
      if(sum_and_dif > biggest_value_sum_and_dif)
         biggest_value_sum_and_dif = sum_and_dif;
      if(sum_and_dif < smallest_value_sum_and_dif)
         smallest_value_sum_and_dif = sum_and_dif;

   }


   //print to terminal
   printf("Biggest value of |A + A|: %u\n"                                                 //biggest_value_sum   
          "Smallest value of |A + A|: %u\n"                                                //smallest_value_sum
          "Biggest value of |A - A|: %u\n"                                                 //biggest_value_dif
          "Smallest value of |A - A|: %u\n"                                                //smallest_value_dif
          "Biggest value of |A + A| - |A - A|: %u\n"                                       //biggest_value_sum_and_dif
          "Smallest value of |A + A| - |A - A|: %d\n",                                     //smallest_value_sum_and_dif 
          biggest_value_sum, smallest_value_sum, biggest_value_dif,
          smallest_value_dif, biggest_value_sum_and_dif, smallest_value_sum_and_dif);

   //print to data file
   if(save_data)
      fprintf(data_file, "%u\t"                                                           //biggest_value_sum
                         "%u\t"                                                           //smallest_value_sum
                         "%u\t"                                                           //biggest_value_dif
                         "%u\t"                                                           //smallest_value_dif
                         "%u\t"                                                           //biggest_value_sum_and_dif
                         "%d\t",                                                          //smallest_value_sum_and_dif
               biggest_value_sum, smallest_value_sum, 
               biggest_value_dif, smallest_value_dif,
               biggest_value_sum_and_dif, smallest_value_sum_and_dif);

   //deallocate memory
   free(result);

   return number_of_mstd_sets;

}

int main(int argc, char **argv){

   short chosen_opt = -1,                                                                 //option chosen by user -> corresponds to the version of MSTD to run
         save_data = 0;                                                                   //boolean variable -> true ( > 0 ) if user wants to save data to a file; false ( <= 0 ) if user wants the opposite
   char *file_name;                                                                       //file name where is to to save data
   unsigned short final_n;                                                                //max value of n to calc
   
   /**************** options ****************/
   int opt,                                                                               //integer where is stored the value returned by getopt_long() function
       user_option_index = 0,                                                             //integer used to know wich option of user_options[] was chosen by the user (used in getopt_long() function)
       number_user_option = 3;                                                            //number of options stored in user_options[]

   struct option user_options[] = {
          {"nb", required_argument, 0, 0},                                                //normal brute force solution
          {"fb", required_argument, 0, 0},                                                //fast brute force solution
          {"save", required_argument, 0, 0},                                              //last one must be always the save option
          {0, 0, 0, 0}
   };

   while((opt = getopt_long(argc, argv, "", user_options, &user_option_index)) != -1){
      switch(opt){
         case 0:
            if(!strcmp(user_options[user_option_index].name,                              //option --save
                       user_options[number_user_option - 1].name)){
               
               int name_size;                                                             //number of characters in document's name where is to be stored the data

               if((name_size = strlen(optarg)))
                  file_name = malloc(name_size + 4);
               else
                  goto error_message;                                                     //this block of code is placed after this while loop, and is where is printed the message explaining how to use this program to the user

               strcpy(file_name, optarg);
               strcat(file_name, ".txt");
               save_data = 1;                                                             //save_data is now true, because the user selected the option --save
            }else
               for(int num = 0; num < number_user_option - 1; num++)                      //other options (--save not included)
                  if(!strcmp(user_options[user_option_index].name, 
                             user_options[num].name)){

                     if(chosen_opt < 0){                                                  //verify if user dont choose more than one option (--save option not included)

                        chosen_opt = num;

                        if((final_n = atoi(optarg)) < MIN_N ||                            //verify if the n's value given by the user does not exceed the predefined range
                            final_n > MAX_N)
                           goto error_message;                                            //this block of code is placed after this while loop, and is where is printed the message explaining how to use this program to the user
                     
                     }else
                        goto error_message;                                               //this block of code is placed after this while loop, and is where is printed the message explaining how to use this program to the user
                  
                  }
            break;                                                                        //break of case 0
         default:
            goto error_message;                                                           //this block of code is placed after this while loop, and is where is printed the message explaining how to use this program to the user
      }
   }

   if(chosen_opt < 0){                                                                    //verify if user chose at least one option (--save option not included)
      error_message:
         fprintf(stderr, "ERROR on input arguments!\n"
                         "Usage: %s [--nb n <or> --fb n] (--save file_name)\n"
                         "  > %-10s normal brute force solution\n"
                         "  > %-10s fastest brute force solution\n"
                         "  > %-10s save data to a txt document\n"
                         "Argements between [] are required (obviously, just one of them),"
                         "and arguments between () are optional\n",
                  argv[0], "--nb", "--fb", "--save");
         return EXIT_FAILURE;                                                             //exit with failure code 
   }
      
  
   /**************** calc MSTD for 1 <= n <= final_n ****************/
   unsigned short n = 0;                                                                  //initialize n's value     
   unsigned int number_of_mstd_sets;                                                      //number of MSTD sets in "current value of n" (this value is updated in the next while loop)
   double ratio_of_mstd_sets,                                                             //proportion between the number of MSTD sets in all calculated sets
          time_to_complete;                                                               //time spent on each mstd() call for the respective value of n
   
   
   if(save_data)                                                                          //open file to save data and print table's first line
      data_file = fopen(file_name, "w");

   //increase n's value until it reaches final_n, and with given n, compute MSTD with the algorithm's version chosen by the user
   do{
      n++;

      //print actual n to terminal
      printf("N -> %d\n", n);

      //print actual n to data file
      if(save_data)
         fprintf(data_file, "%d\t", n);

      //compute MSTD
      elapsed_time();
      number_of_mstd_sets = mstd(n, save_data, chosen_opt);
      time_to_complete = elapsed_time();
      ratio_of_mstd_sets = number_of_mstd_sets/pow(2, n - 1);

      //print to terminal
      printf("Number of MSTD sets: %u\n"
             "Ratio of MSTD sets: %f\n"
             "Time: %f\n\n\n", 
             number_of_mstd_sets, ratio_of_mstd_sets, time_to_complete);

      //print to data file
      if(save_data)
         fprintf(data_file, "%u\t"                                                        //number_of_mstd_sets
                            "%f\t"                                                        //ratio_of_mstd_sets
                            "%f\n",                                                       //time_to_complete
                  number_of_mstd_sets, ratio_of_mstd_sets, time_to_complete);

   }while(n < final_n);

   //close data file
   if(save_data)
      fclose(data_file);

   return EXIT_SUCCESS;

}