# Script for testing sorting algorithms.
# The `PROG' variable must be defined in order for this script to know which algorithm
# it should test.

. "./tests/init.sh"

[ -z "$PROG" ] && fatal "PROG variable must be defined."

echo
echo "Testing program $PROG"
echo


# tests a the a given sorting program correctly sorts a set of sequential integers.
# To determine that a program is correct, its output is compared to that of
# UNIX `sort' command.
#
# Parameters:
#
#   $1 - the start of the integer sequence
#   $2 - the sequece increment
#   $3 - the sequence final element
seq_sort() {
  echo -ne "\tFrom $1 to $3: "

  local program_output=$(seq "$1" "$2" "$3" | $PROG)
  local sort_output=$(seq "$1" "$2" "$3" | sort -n)

  if cmp -s <(echo $program_output) <(echo $sort_output); then
    echo "OK"
  else
    echo "Fail"
  fi
}

# tests a the a given sorting program correctly sorts a set of randomly chosen integers.
# To determine that a program is correct, its output is compared to that of
# UNIX `sort' command.
#
# Parameters:
#
#   $1 - the number of random numbers to be sorted
rand_sort() {
  echo -ne "\t$1 random numbers: "
  local random

  for (( i = 0; i < $1; i++)); do
    random=$(echo -e "$random\n$RANDOM")
  done

  local program_output=$(echo "$random" | $PROG)
  local sort_output=$(echo "$random" | sort -n)

  if cmp -s <(echo $program_output) <(echo $sort_output); then
    echo "OK"
  else
    echo "Fail"
  fi
}

echo "Small problems"
echo "=============="
echo

seq_sort "1" "1" "100"
seq_sort "100" "-1" "0"
rand_sort "10"
rand_sort "50"
rand_sort "100"
echo

echo "Large problems"
echo "=============="
echo

seq_sort "1" "1" "10000"
seq_sort "10000" "-1" "0"
rand_sort "1000"
rand_sort "5000"
rand_sort "10000"

echo
exit 0
