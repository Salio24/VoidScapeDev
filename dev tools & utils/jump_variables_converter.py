import math

def compute(tickrate):
    print("What to compute? 1 for Total ticks : 2 for Initial impulse : 3 for Continuous acceleration : 4 for Total jump height")
    type = int(input("¿? : "))

    if (type == 1):
        gd = float(input("Gravity down: "))
        gu = float(input("Gravity up: "))
        i1 = float(input("Initial impulse: "))
        i2 = float(input("Continuous acceleration: "))
        h =  float(input("Total jump height: "))

        I1 = i1 + gd
        I2 = i2 + gu

        dt = 1 / tickrate

        ticks = (-2 * I1 * (1 + (I2 / abs(gu))) + math.sqrt(pow((2 * I1 * (1 + (I2 / abs(gu)))), 2) - 4 * I2 * (1 + (I2 / abs(gu))) * (I1 + ((I1 * I1) / abs(gu)) - ((8 * h) / (dt * dt))))) / (2 * I2 * (1 + (I2 / abs(gu))))

        print("Total ticks: " + str(ticks))
        
        
    elif (type == 2):
        gd = float(input("Gravity down: "))
        gu = float(input("Gravity up: "))
        i2 = float(input("Continuous acceleration: "))
        h =  float(input("Total jump height: "))
        n =  int(input("Total ticks: "))
        
        I2 = i2 + gu
        
        n = n + 1
        
        dt = 1 / (tickrate * 2);
        
        A1 = 1.0
        B1 = 2 * (n - 1) * I2 + (2 * n - 1) * abs(gu)
        C1 = ((n - 1)**2 * I2**2) + ((n - 1)**2 * I2 * abs(gu)) - (2 * h * abs(gu)) / (dt**2)

        discriminant = B1**2 - 4 * A1 * C1
        sqrt_discriminant = math.sqrt(discriminant)

        i1 = (-B1 + sqrt_discriminant) / (2 * A1) - gd
    
        print("Initial impulse: " + str(i1))
        
        
    elif (type == 3):
        gd = float(input("Gravity down: "))
        gu = float(input("Gravity up: "))
        i1 = float(input("Initial impulse: "))
        h =  float(input("Total jump height: "))
        n =  int(input("Total ticks: "))

        
        I1 = i1 + gd
        
        n = n + 1
        
        dt = 1 / (tickrate * 2);
        
        A2 = (n - 1)**2
        B2 = (n - 1)**2 * abs(gu) + 2 * (n - 1) * I1
        C2 = (2 * n - 1) * I1 * abs(gu) + I1**2 - (2 * h * abs(gu)) / (dt**2)

        discriminant = B2**2 - 4 * A2 * C2
        sqrt_discriminant = math.sqrt(discriminant)

        i2 = (-B2 + sqrt_discriminant) / (2 * A2) - gu
        
        print("Continuous acceleration: " + str(i2))
        
        
    elif (type == 4):
        gd = float(input("Gravity down: "))
        gu = float(input("Gravity up: "))
        i1 = float(input("Initial impulse: "))
        i2 = float(input("Continuous acceleration: "))
        n =  int(input("Total ticks: "))

        I1 = i1 + gd
        I2 = i2 + gu

        n = n + 1

        dt = 1 / (tickrate * 2);

        va = (I1 + (n - 1) * I2) * dt;

        xi = (((I1 * dt * dt) / 2) + ((I1 * dt + va) / 2) * (n - 1) * dt);

        xf = (va * va) / (2 * abs(gu))

        h = xi + xf;

        print("Total jump height: " + str(h))
        

def main():
    print("---------------------------------------------------------------------------------------------------------------------------------")
    print("| Be aware that these formulas are just approximations and they have some error that can range from 0.01 to 1                   |")
    print("| I found option 1 to have the lowest error, option 4 around 0.01, options 2, 3 up to 1. Game itself has an error of < 0.000001 |")
    print("| I assume error may be more or less depending on specific numbers, tho error in this case is static.                           |")
    print("| When you find numbers that fit, recheck them manually in game, just to be sure                                                |")
    print("---------------------------------------------------------------------------------------------------------------------------------")

    tickrate = 128

    #tickrate = float(input("Tickrate ?: "))
    
    tickrate = tickrate / 2

    while (1):
        compute(tickrate)
        
        print("Again? 1 yes : anything else no")
        ccc = input("¿? : ")
        try:
            if (int(ccc) == 1):
                continue
            else:
                print("ggwp")
                break
        except:
            print("ggwp")
            break


main()

