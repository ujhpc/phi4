BEGIN {v=0.038411234; vn=n*v;}

$1!~/#/ {print $1-vn}

