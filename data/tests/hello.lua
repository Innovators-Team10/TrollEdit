secretword = "llama" --the secret word

write ("What is your name? ")
name = read ("*l")

print ("Hello " .. name .. "!") --ordinary greeting [!528,66,1!] 
write ("What is the secret word? ")
guess = read ("*l")
while guess ~= secretword do
    write ("Wrong, try again. What is the secret word? ")
    guess = read ("*l")
end
