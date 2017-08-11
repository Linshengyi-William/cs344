#!/bin/bash
# PROGRAM 1 
# Author: James Palmer
# 1/29/17



#Setup for temp files and also trap for unexpected interruptions

avg=avg1$$
med=med1$$
results=results$$
TMP_FILE=tempFile$$

trap "rm -f $avg $med $results $TMP_FILE; exit 1" INT HUP TERM

#Assignment outline said calculate Rows before Columns
#hence, function for finding the Rows

select_rows()
{
    echo -e "Average\tMedian"   

    while read myLine
    do
        # based off this weeks class video titled: tracking variables 
        sum=0 
        count=0 
        average=0
    
        
        #Added a sort off canvas for median value
        
        sortedLine=$( echo $myLine | tr " " "\n" | sort -g )

        # Loop to do two things:
        # Sum of each row then counts values in said row
		#Each line gets sorted from this point
        for i in $sortedLine
        do
            num=$i
            count=$[ $count + 1 ]
            sum=$[ $sum + $num ]
        done

        # Way to find the median, with way of counting

        halfCount=`expr $[ $count / 2 ] + 1`
        x=1  

        for y in $sortedLine
        do
            num2=$y
            if [ $x -le $halfCount ]
            then
                median=$num2
                x=$[ $x + 1 ]
            fi
        done


        # Equation below is as follows: Average = Sum of Row /Count of Row.
        # Personal note: had to float equation initially to default from errors, then add .5 followed by integer division 

        if [ $count -eq 0 ]
        then
            average=""
        else
            average=$( echo "scale=2;  $sum / $count" | bc )
            average=$( echo "($average + .5) / 1" | bc )
        fi

        #Print the final solution here
        echo -e "$average\t$median"

    done < $user_file

}

#Part 2 of assignment:
#function for finding the Columns

select_col()
{
    # based off of videos: tracking variables 
    col=1
    j=0
   

    # Pull number of columns from user file with 'w count'
    num_col=$( head -n 1 $user_file | wc -w )
   
 
    #Loop through given Columns then cut/move to next column

    while [ $j -lt $num_col ]
    do
        sum=0
        count=0
        average=0

        #added sort line here for calculating
        sortedLine=$( cut -f $col $user_file | tr " " "\n" | sort -g )
        

        #Add a loop based off the sorted line to find the sum
        for i in $sortedLine
        do
            sum=`expr $sum + $i`
            count=`expr $count + 1`
        done


        # Find each column's average
        if [ $count -eq 0 ]
        then
            average=""
        else
            average=$( echo "scale=2;  $sum / $count" | bc )
            average=$( echo "($average + .5) / 1" | bc )
        fi


        # Below I find the medium then sort each line

        halfCount=`expr $[ $count / 2 ] + 1`
        x=1  #counter

        for y in $sortedLine
        do
            num2=$y
            if [ $x -le $halfCount ]
            then
                median=$num2
                x=$[ $x + 1 ]
            fi
        done

        #Here I send the average and new found medians to the output file
        echo $average >> $avg
        echo $median >> $med

        #have to increment each column to cut and start a new line
		#what is another way to impliment this earlier in the code, if exists? Doing research still.....
        col=`expr $col + 1`
        j=`expr $j + 1`
    done
    

    # This is a final output section 
  
    echo -e "Averages: " > $results
    if [ -e $avg ]
    then
        tr "\n" "\t" < $avg >> $results
    fi
    echo -e "\n" >> $results

    echo -e "Medians: " >> $results
    if [ -e $med ]
    then
        tr "\n" "\t" < $med >> $results
    fi
    echo -e "\n" >> $results
	
	#cleaning and removing temp files to prevent errors.
    #remove said temp files here with the names: "avg" and "med"
    rm -f $avg $med

    #remove other temp files labeled: "results"
    cat $results
    rm -f $results
}


#someone talked about in canvas of an error proofing if the wrong file is mentioned to this .sh file
#if so, this will allow the data to be stored in a temp file and can be accesed like a regular file

no_file()
{
    while read line
    do 
        echo -e "$line" >> $TMP_FILE
    done < /dev/stdin
	
    user_file=$TMP_FILE	
}
#second note, this helped with added the last testing file given on the homework page


#using the same variable names as given by homework files
usage_message=$( echo "Usage: stats {-rows|-cols} [file]" )
error_message=$( echo "stats: cannot read $2" )


#final checking for where to send information and their parameters 

if [ "$#" -lt 1 ] || [ "$#" -gt 2 ]
then
	echo $usage_message >&2
	exit 1	
fi

#added this section for if and only if, a second parameter was implimented

#explained to me by online TA hours that this will make sure the second parameter 
#will exist and show, why I need this section

if [[ "$2" != "" ]]
then
	if [ ! -r "$2" ]
	then
		echo $error_message >&2
		exit 1
	
	else
		user_file=$2
	fi
else
	no_file
fi


#rows and columns from our function calculated here
#changed from the given code from Assignment 1 page

if [[ "$1" == -r* ]]
then
	select_rows
elif [[ "$1" == -c* ]]
then
	select_col
else
	echo $usage_message >&2
	exit 1	
	
fi

#kept having an error dump at time so added the following section
#it should dump the last std temp file
rm -f $TMP_FILE
