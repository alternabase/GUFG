#!/bin/bash
awk '{
	delta = match($0, /\$D/)
	if(delta){
		print $1"\t" 2*$2, 2*$3, $4, $5
		if($6 || $7 || $8 || $9)
			print "\t" 2*$6, 2*$7, $8, $9
		if($10 || $11 || $12 || $13)
			print "\t" 2*$10, 2*$11, $12, $13
		if($14 || $15 || $16 || $17)
			print "\t" 2*$14, 2*$15, $16, $17
		if($18 || $19 || $20 || $21) 
			print "\t" 2*$18, 2*$19, $20, $21
		if($22 || $23 || $24 || $25) 
			print "\t" 2*$22, 2*$23, $24, $25
		if($26 || $27 || $28 || $29) 
			print "\t" 2*$26, 2*$27, $28, $29
		if($30 || $31 || $32 || $33)
			print "\t" 2*$30, 2*$31, $32, $33
	} else {
		rects = match($0, /\$(A|R|C)/)
		if(rects) {
			print $1"\t" 2*$2, 2*$3, 2*$4, 2*$5
			if($6 || $7 || $8 || $9)
				print "\t" 2*$6, 2*$7, 2*$8, 2*$9
			if($10 || $11 || $12 || $13)
				print "\t" 2*$10, 2*$11, 2*$12, 2*$13
			if($14 || $15 || $16 || $17)
				print "\t" 2*$14, 2*$15, 2*$16, 2*$17
			if($18 || $19 || $20 || $21) 
				print "\t" 2*$18, 2*$19, 2*$20, 2*$21
			if($22 || $23 || $24 || $25) 
				print "\t" 2*$22, 2*$23, 2*$24, 2*$25
			if($26 || $27 || $28 || $29) 
				print "\t" 2*$26, 2*$27, 2*$28, 2*$29
			if($30 || $31 || $32 || $33)
				print "\t" 2*$30, 2*$31, 2*$32, 2*$33
		} else {
			effect = match($0, /SpawnsOn|SpawnPosition|Proximity|Position|Lift|Push|Blowback/, e)
			if(effect) {
				print $1, 2*$2
				if($3) print " " $3, 2*$4
				if($5) print " " $5, 2*$6
				if($7) print " " $7, 2*$8
				if($9) print " " $9, 2*$10
				if($11) print " " $11, 2*$12
				if($13) print " " $13, 2*$14
			} else {
				print $0
			}
		}
	}
}' $1 > $1.dbl

sed ':a;N;$!ba;s/\n\t/\t/g' $1.dbl | sed ':a;N;$!ba;s/\n\ /\ /g' > $1

rm $1.dbl

#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
