## Creator: James Palmer
## Assignment: Project 5

import random
import string

content = 'Three created file names:'
math = 'Two variables with their product:'
var1 = 'First Number:'
var2 = 'Second Number:'
solution = 'Product of both numbers:'

#labeling files
print (content)

for i in range(1, 4):

    #creat 3 file names with random lettering
    file = "random_string" + str(i)
	#open file and write string
    f = open(file, 'w')
    randomString = ""

    #assignment requires 10 letters lowercase with no spaces followed by a 11th newline character
    for j in range(0, 10):

        randomString += random.choice(string.ascii_lowercase)

    #process the random generated string to file
    f.write(randomString)

    # close file
    f.close()

    # print random string 
    print(randomString)

#some whitespace for clarity
print (' ')
print (math)
# Part 2: generate two random integers ranging 1 to 42 then print said numbers
first = random.randint(1, 42)
second = random.randint(1, 42)

#added output text
print (var1, first)
print(var2, second)

#as per assignment print out the product of the two numbers
product = first * second
print (solution, product)