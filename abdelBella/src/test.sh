#!/bin/bash

echo "debut du test"

server_address='10.43.11.145'
filename='cna.pdf'
port='5000'
echo -n "window/timeout ," >> value.csv
for t in `seq 10000 10000 100000`
do
    if [ $t = 100000 ]; 
    then
        echo $t >> value.csv
    else
        echo -n $t" ," >> value.csv
    fi
done

for k in `seq 10 10 100`
do
    echo -n $k" ," >> value.csv
    for t in `seq 10000 10000 100000`
    do
        ../bin/serveur2-abdelBella 5000 ${k} ${t}>sever.log &
        sleep 1
        somme='0'

        for i in `seq 10`;
        do
            echo "Sending "$i" time file... for window "$k" and timeout"$t
            (time ../../client1 ${server_address} ${port} ${filename} 0) 2> time.log 
            sed -i 's/,/./' time.log 

            sizeFile=$(stat --printf="%s" $filename)
            timeTrans=$(grep 'real' time.log | cut -d$'\t' -f2)
            min=$(grep 'real' time.log | cut -d$'\t' -f2 | cut -d'm' -f1)
            sec=$(grep 'real' time.log | cut -d$'\t' -f2 | cut -d'm' -f2 | cut -d's' -f1)


            #sec=$(('scale=3;'"$sec + 60" | bc -l))
            sec=$(bc <<< "scale=2;$sec+60*$min")
            #echo "sizeFileInit:"$sizeFile" octets"
            #echo "time:"$timeTrans
            #echo "min:"$min" min"
            #echo "sec:"$sec" secs"

            debit=$(bc <<< "scale=3;($sizeFile/$sec)/1000")
            somme=$(bc <<< "scale=3;$somme+$debit")
            #echo "debit: "$debit" ko/s"

            prefixe="copy_"
            newFilename=$prefixe$filename
            sizeFileReceive=$(stat --printf="%s" $newFilename)
            #echo "sizeFileReceive:"$sizeFileReceive" octets"

            rm time.log

            if [ $sizeFileReceive = $sizeFile ];
            then
            echo "Test Passed : Size match"
            else
                echo "Test Failed : Error in file size"
            fi
            echo "fin du test"
        done

        moyenne=$(bc <<< "scale=3;$somme/10")
        #echo "Moyenne : "$moyenne
        if [ $t = 100000 ]; 
        then
            echo $moyenne >> value.csv
        else
            echo -n $moyenne"," >> value.csv
        fi
        
        killall serveur2-abdelBella
    done
done
