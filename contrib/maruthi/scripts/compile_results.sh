#!/bin/bash


# graphs[0]=rng
# graphs[1]=beta17
# graphs[2]=beta15
# graphs[3]=beta14
# graphs[4]=beta13
# graphs[5]=beta11
# graphs[6]=gg
# graphs[7]=beta95
# graphs[8]=beta9
# graphs[9]=beta8
# graphs[10]=beta5

# graphs[0]=knn10
# graphs[1]=knn15
# graphs[2]=knn20
# graphs[3]=knn25
# graphs[4]=knn30
# graphs[5]=knn40
# graphs[6]=knn50
# graphs[7]=knn70
# graphs[8]=knn90
# graphs[9]=knn100

# graphs[0]=hybrid_10
# graphs[1]=hybrid_15
# graphs[2]=hybrid_20
# graphs[3]=hybrid_25
# graphs[4]=hybrid_30
# graphs[5]=hybrid_40
# graphs[6]=hybrid_50
# graphs[7]=hybrid_70
# graphs[8]=hybrid_90
# graphs[9]=hybrid_100



# graphs[0]=knn5
# graphs[1]=knn10
# graphs[2]=knn15
# graphs[3]=knn20
# graphs[4]=knn25
# graphs[5]=knn30
# graphs[6]=knn40
# graphs[7]=knn50

graphs[0]=gg
# graphs[1]=beta14
# graphs[2]=beta11
# graphs[3]=gg
# graphs[7]=beta95
# graphs[8]=beta9
# graphs[9]=beta8
# graphs[10]=beta5

for i in "${graphs[@]}"
do
    file=`ls "$i"_*.txt`
    python /home/mn/lemsvxl/src/contrib/maruthi/scripts/checkVisits.py $1 $file
done
