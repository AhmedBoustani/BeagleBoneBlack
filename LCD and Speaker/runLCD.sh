pipe=pipe

# To fix the clock skew detected warning
find . -type f | xargs -n 5 touch
make clean #Clean
make # Compile everything

# Remove any previous pipes with the same name
trap "rm -f $pipe" EXIT

# Creates pipe
if [ ! -e $pipe ]; then
  mkfifo $pipe
  echo "A pipe has been created with the name $pipe"
fi

# To use the pipe in the same terminal where it was created
exec 3<> $pipe

# Initialize the LCD
./InitLCD

echo "Enter your text" 
while true
do
  if read line; then
    if [ "$line" = 'quit' ]; then
      break
    fi
    # Write the line taken from terminal to the pipe
    echo $line >$pipe
    # Start the program to communicate with LCD
    ./TalkToLCD 
  fi
done

echo "Reader exiting"

