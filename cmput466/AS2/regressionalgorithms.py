from __future__ import division  # floating point division
import numpy as np
import math

import utilities as utils
EPOCHS = 200
class Regressor:
    """
    Generic regression interface; returns random regressor
    Random regressor randomly selects w from a Gaussian distribution
    """

    def __init__( self, parameters={} ):
        """ Params can contain any useful parameters for the algorithm """
        self.params = {}
        self.reset(parameters)

    def reset(self, parameters):
        """ Reset learner """
        self.weights = None
        self.resetparams(parameters)

    def resetparams(self, parameters):
        """ Can pass parameters to reset with new parameters """
        self.weights = None
        try:
            utils.update_dictionary_items(self.params,parameters)
        except AttributeError:
            # Variable self.params does not exist, so not updated
            # Create an empty set of params for future reference
            self.params = {}

    def getparams(self):
        return self.params

    def learn(self, Xtrain, ytrain):
        """ Learns using the traindata """
        self.weights = np.random.rand(Xtrain.shape[1])

    def predict(self, Xtest):
        """ Most regressors return a dot product for the prediction """
        ytest = np.dot(Xtest, self.weights)
        return ytest

class RangePredictor(Regressor):
    """
    Random predictor randomly selects value between max and min in training set.
    """

    def __init__( self, parameters={} ):
        """ Params can contain any useful parameters for the algorithm """
        self.params = {}
        self.reset(parameters)

    def reset(self, parameters):
        self.resetparams(parameters)
        self.min = 0
        self.max = 1
        
    def learn(self, Xtrain, ytrain):
        """ Learns using the traindata """
        self.min = np.amin(ytrain)
        self.max = np.amax(ytrain)

    def predict(self, Xtest):
        ytest = np.random.rand(Xtest.shape[0])*(self.max-self.min) + self.min
        return ytest

class MeanPredictor(Regressor):
    """
    Returns the average target value observed; a reasonable baseline
    """
    def __init__( self, parameters={} ):
        self.params = {}
        self.reset(parameters)

    def reset(self, parameters):
        self.resetparams(parameters)
        self.mean = None
        
    def learn(self, Xtrain, ytrain):
        """ Learns using the traindata """
        self.mean = np.mean(ytrain)

    def predict(self, Xtest):
        return np.ones((Xtest.shape[0],))*self.mean


class FSLinearRegression(Regressor):
    """
    Linear Regression with feature selection, and ridge regularization
    """
    def __init__( self, parameters={} ):
        self.params = {'features': [1,2,3,4,5]}
        self.reset(parameters)

    def learn(self, Xtrain, ytrain):
        """ Learns using the traindata """
        # Dividing by numsamples before adding ridge regularization
        # to make the regularization parameter not dependent on numsamples
        numsamples = Xtrain.shape[0]
        Xless = Xtrain[:,self.params['features']]
        self.weights = np.dot(np.dot(np.linalg.inv(np.dot(Xless.T,Xless)/numsamples), Xless.T),ytrain)/numsamples
        # print (self.weights.shape)

    def predict(self, Xtest):
        Xless = Xtest[:,self.params['features']]
        ytest = np.dot(Xless, self.weights)
        # print (ytest.shape)

        return ytest

class RidgeLinearRegression(Regressor):
    """
    Linear Regression with ridge regularization (l2 regularization)
    TODO: currently not implemented, you must implement this method
    Stub is here to make this more clear
    Below you will also need to implement other classes for the other algorithms
    """
    def __init__( self, parameters={} ):
        # Default parameters, any of which can be overwritten by values passed to params
        self.params = {'regwgt': 0.5}
        self.features = 385
        self.reset(parameters)
        self.lammbda = 0.01
        self.previous_weight =  np.identity(self.features)


    def learn(self, Xtrain, ytrain):
        """ Learns using the traindata """
        # Dividing by numsamples before adding ridge regularization
        # to make the regularization parameter not dependent on numsamples
        numsamples = Xtrain.shape[0]
        # Xless = Xtrain[:,range(self.features)]
        # print(Xless.shape)

        self.weights = np.dot(np.dot(np.linalg.inv(np.dot(Xtrain.T,Xtrain) + self.previous_weight * self.lammbda), Xtrain.T),ytrain)
        np.fill_diagonal(self.previous_weight,utils.l2(self.weights))

        # temp = np.dot(Xtrain,self.Wmap) - ytrain

        # self.weights = np.dot(temp.T,temp) + self.lammbda * np.dot(self.Wmap.T,self.Wmap)


    def predict(self, Xtest):
        Xless = Xtest[:,range(self.features)]
        ytest = np.dot(Xless, self.weights)
        return ytest



class Lasso(Regressor):
    def __init__( self, parameters={} ):
        # Default parameters, any of which can be overwritten by values passed to params
        self.params = {}
        self.features = 385
        self.reset(parameters)
        self.lammbda = 0.01


    def learn(self, Xtrain, ytrain):
        """ Learns using the traindata """
        self.weights =  np.dot(
                np.dot(np.linalg.inv(np.dot(Xtrain.T, Xtrain) + np.identity(self.features) * self.lammbda), Xtrain.T),
                ytrain)
        err = 10000
        tolerance = 10*math.exp(-4)
        max = 10*math.exp(5)
        numsamples = Xtrain.shape[0]

        x1 = np.dot(Xtrain.T,Xtrain)/numsamples
        x2 = np.dot(Xtrain.T,ytrain)/numsamples
        stepsize = 1 / (2 * np.linalg.norm(x1,'fro'))
        temp = np.dot(Xtrain,self.weights) - ytrain
        cw = np.dot(temp.T,temp)/(2*numsamples)
        i = 0
        while abs(cw - err) > tolerance and i < max:
            i+=1
            err = cw

            wi = self.weights - stepsize * np.dot(x1 , self.weights) + stepsize * x2
            for j in range (len(wi)):

                if wi[j] > stepsize * self.lammbda:
                    self.weights[j] = wi[j] - stepsize * self.lammbda

                elif abs(wi[j]) <= stepsize * self.lammbda:
                    self.weights[j] = 0
                elif wi[j] < stepsize * (-self.lammbda):
                    self.weights[j] = wi[j] + stepsize * self.lammbda
            # print (i)
            a = np.dot(Xtrain, self.weights)
            # print (a.shape)
            # print(ytrain.shape)
            temp = a - ytrain
            cw = np.dot(temp.T, temp) + self.lammbda * utils.l1(self.weights)



    def predict(self, Xtest):
        Xless = Xtest[:,range(self.features)]
        ytest = np.dot(Xless, self.weights)
        return ytest



class SGD(Regressor):


        def __init__(self, parameters={}):
            self.features = 385
            self.reset(parameters)
            self.stepsize = 0.01
            self.epochs = EPOCHS


        def learn(self, Xtrain, ytrain):
            """ Learns using the traindata """
            self.weights = np.random.rand(self.features,)
            num = Xtrain.shape[1]

            for i in range(self.epochs) :

                # shuffle
                seq = np.random.permutation(num)
                nx = []
                ny = []
                for s in seq:
                    nx.append(Xtrain[s])
                    ny.append(ytrain[s])

                for j in range(num):
                    g = np.dot(np.dot(nx[j].T,self.weights) - ny[j],nx[j])
                    self.weights = self.weights - self.stepsize * g

        def predict(self, Xtest):
            Xless = Xtest[:, range(self.features)]
            ytest = np.dot(Xless, self.weights)
            return ytest



class batchGD(Regressor):


        def __init__(self, parameters={}):
            self.features = 385
            self.reset(parameters)
            self.epochs = 1000

        def linesearch(self,wt,g,x,y):
            maxstepsize = 1.0
            t = 0.7
            max = 10 * math.exp(5)
            tolerance = 10 * math.exp(-4)
            stepsize = maxstepsize
            w = wt
            temp = np.dot(x, w) - y
            cw = utils.l2(temp)/(2*self.features)
            obj = cw
            i = 0
            while i < max:
                i += 1
                w = wt - stepsize * g
                temp = np.dot(x, w) - y
                cw = utils.l2(temp) / (2 * self.features)
                if cw < (obj - tolerance):
                    break
                stepsize = t * stepsize
                if i >= max:
                    stepsize = 0

            return stepsize



        def learn(self, Xtrain, ytrain):
            """ Learns using the traindata """
            self.weights = np.random.rand(self.features,)
            err = 10000
            tolerance = 10 * math.exp(-4)
            max = EPOCHS

            num = Xtrain.shape[1]

            # stepsize = 1 / (2 * np.linalg.norm(x1, 'fro'))
            temp = np.dot(Xtrain, self.weights) - ytrain
            cw = utils.l2(temp)/(2*num)
            i = 0
            while abs(cw - err) > tolerance and i < max:
                i += 1
                err = cw
                g = np.dot(Xtrain.T,temp)/num
                stepsize = self.linesearch(self.weights,g,Xtrain,ytrain)
                self.weights = self.weights - stepsize * g
                temp = np.dot(Xtrain, self.weights) - ytrain
                cw = utils.l2(temp) / (2 * num)

        def predict(self, Xtest):
            Xless = Xtest[:, range(self.features)]
            ytest = np.dot(Xless, self.weights)
            return ytest