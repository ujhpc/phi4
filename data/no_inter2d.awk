BEGIN {v=0.137382828806; vn=n*v;}

$1!~/#/ {print $1-vn}

