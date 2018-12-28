import os
import numpy as np
import random



class NeuralNetwork(object):

    def __init__(self, sizes, learning_rate=0.1, batchsize=4,
                 epochs=100,gradientdescent = 'sgd'):



        self.sizes = sizes

        # init weight
        self.weights1 =  np.random.randn(sizes[1], sizes[0])
        self.weights2 =  np.random.randn(sizes[2], sizes[1])


        # init hidden layers.
        self.hid = [1,2]
        # init output layers
        self.outs = [1,2]

        self.batchsize = batchsize
        self.epochs = epochs
        self.lr = learning_rate
        self.accuracy = []

    # activate functions
    def sigmoid(self,a):
        return 1.0 / (1.0 + np.exp(-a))
    def softmax(self,a):
        return np.exp(a) / np.sum(np.exp(a))

    # cross Entropy
    def handle(self,a):
        return a * (1 - a)

    def forward(self, x):

        self.hid[0] = self.weights1.dot(x)
        self.outs[0] = self.sigmoid(self.hid[0])
        self.hid[1] = self.weights2.dot(self.outs[0])
        self.outs[1] = self.softmax(self.hid[1])

    def backward(self, x, y):

        nabla = [np.zeros(self.weights1.shape),np.zeros(self.weights2.shape)]
        error = (self.outs[1] - y) * self.handle(self.softmax(self.hid[1]))
        nabla[1] = error.dot(self.outs[0].transpose())
        error = np.multiply(self.weights2.transpose().dot(error),self.handle(self.sigmoid(self.hid[0])))
        nabla[0] = error.dot(x.transpose())

        return nabla

    def learn(self, training_data,training_result, validation_data,validation_result):


        nums = np.arange(len(training_data))

        for epoch in range(self.epochs):
            random.shuffle(nums)

            nablaw1 = np.zeros(self.weights1.shape)
            nablaw2 = np.zeros(self.weights2.shape)
            for i in range(len(training_data)):

                self.forward(training_data[nums[i]])
                delta_nabla_w = self.backward(training_data[nums[i]], training_result[nums[i]])
                nablaw1 = nablaw1 + delta_nabla_w[0]
                nablaw2 = nablaw2 + delta_nabla_w[1]
                if i % self.batchsize == 0:

                    self.weights1 = self.weights1 - (self.lr / self.batchsize) * nablaw1
                    self.weights2 = self.weights2 - (self.lr / self.batchsize) * nablaw2

                    nablaw1 = np.zeros(self.weights1.shape)
                    nablaw2 = np.zeros(self.weights2.shape)

            accuracy = self.validate(validation_data,validation_result)/ 100.0
            print("Epoch: "+str(epoch + 1)+ "     accuracy: " +str(accuracy)  +"%.")
            self.accuracy.append(accuracy)
        return self.accuracy


    def validate(self, validation_data,validation_result):

        results = [(self.predict(x) == y) for x, y in zip(validation_data,validation_result)]
        return sum(results)

    def predict(self, x):

        self.forward(x)
        return np.argmax(self.outs[1])


