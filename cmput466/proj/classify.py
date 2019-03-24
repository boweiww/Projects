import sys
from sklearn.datasets import load_digits  #
import pylab as pl
from sklearn.model_selection import train_test_split  #
from sklearn.preprocessing import StandardScaler  #
from sklearn.svm import LinearSVC
from sklearn.metrics import classification_report
from sklearn.neighbors import KNeighborsClassifier
from ReadImage import compressimage,readImage,readrawImage
from sklearn.metrics import accuracy_score
import time
import numpy as np
import keras
from keras.datasets import cifar10
from keras.models import Sequential
from keras.layers import Dense, Dropout, Activation, Flatten
from keras.layers import Conv2D, MaxPooling2D

def compress():
    # the image will be compressed from 2048x1536 to 409x307

    start = time.time()
    train0 = compressimage('train/0')
    print(train0)

    train1 = compressimage('train/1')
    print(train1)
    valid0 = compressimage('valid/v0')
    print(valid0)
    valid1 = compressimage('valid/v1')
    print(valid1)

    test0 = compressimage('test/t0')
    print(test0)
    test1 = compressimage('test/t1')
    print(test1)
    end = time.time()
    print(end - start)


def unison_shuffled_copies(a, b):
    assert len(a) == len(b)
    p = np.random.permutation(len(a))
    return a[p], b[p]


def svm(X_train, X_test, Y_train, Y_test):
    ss = StandardScaler()
    X_train = ss.fit_transform(X_train)
    X_test = ss.transform(X_test)

    # l1
    # lsvc = LinearSVC(penalty='l1')

    # l2
    lsvc = LinearSVC()

    lsvc.fit(X_train, Y_train)

    Y_predict = lsvc.predict(X_test)
    # print(digits.target_names.astype(str))
    print("The accuracy of svm is  {}%".format(accuracy_score(Y_test, Y_predict)))
    print(classification_report(Y_test, Y_predict))
    '''The reported averages include micro average (averaging the total true positives, false negatives and false positives)
    macro average (averaging the unweighted mean per label), 
    weighted average (averaging the support-weighted mean per label) 
    and sample average (only for multilabel classification).'''


def knn(X_train, X_test, Y_train, Y_test):

    knn_clf = KNeighborsClassifier()
    knn_clf.fit(X_train, Y_train)
    KNeighborsClassifier()
    Y_predict = knn_clf.predict(X_test)


    print("The accuracy of knn is {}%".format(accuracy_score(Y_test, Y_predict)))

    # print(knn_clf.score(X_test, Y_test))
    print(classification_report(Y_test, Y_predict))

def readtrain():
    image0 = readImage('newtrain/0/')
    image1 = readImage('newtrain/1/')
    image = image0 + image1
    image = np.array(image, ndmin=2)
    target = np.concatenate((np.zeros(len(image0)), np.ones(len(image1))))
    image, target = unison_shuffled_copies(image, target)


    X_train, X_test, Y_train, Y_test = train_test_split(image, target, test_size=0.25, random_state=33)
    return X_train, X_test, Y_train, Y_test

def readValid():
    image0 = readImage('newtrain/0/')
    image1 = readImage('newtrain/1/')
    image = image0 + image1
    image = np.array(image, ndmin=2)
    target = np.concatenate((np.zeros(len(image0)), np.ones(len(image1))))
    image, target = unison_shuffled_copies(image, target)
    X_train = image
    Y_train = target
    image0 = readImage('newvalid/0/')
    image1 = readImage('newvalid/1/')
    image = image0 + image1
    X_test = np.array(image,ndmin=2)
    Y_test = np.concatenate((np.zeros(len(image0)),np.ones(len(image1))))
    return X_train, X_test, Y_train, Y_test

def readtftrain():
    image0 = readrawImage('newtrain/0/')
    image1 = readrawImage('newtrain/1/')
    image = image0 + image1
    image = np.array(image, ndmin=2)
    target = np.concatenate((np.zeros(len(image0)), np.ones(len(image1))))
    image, target = unison_shuffled_copies(image, target)
    X_train, X_test, Y_train, Y_test = train_test_split(image, target, test_size=0.25, random_state=33)
    # Y_train = np.array(Y_train, ndmin=2)
    # Y_train = np.transpose(Y_train)
    return X_train, X_test, Y_train, Y_test

def readtfValid():
    image0 = readrawImage('newtrain/0/')
    image1 = readrawImage('newtrain/1/')
    image = image0 + image1
    image = np.array(image, ndmin=2)
    target = np.concatenate((np.zeros(len(image0)), np.ones(len(image1))))
    image, target = unison_shuffled_copies(image, target)
    X_train = image
    Y_train = target
    image0 = readrawImage('newvalid/0/')
    image1 = readrawImage('newvalid/1/')
    image = image0 + image1
    X_test = np.array(image,ndmin=2)
    Y_test = np.concatenate((np.zeros(len(image0)),np.ones(len(image1))))
    return X_train, X_test, Y_train, Y_test


def CNN(x_train, x_test,y_train, y_test):
    num_classes = 2
    # model_name = 'cifar10.h5'

    # The data, shuffled and split between train and test sets:
    # (x_train, y_train), (x_test, y_test) = cifar10.load_data()

    x_train = x_train.astype('float32') / 255
    x_test = x_test.astype('float32') / 255
    # Convert class vectors to binary class matrices.
    y_train = keras.utils.to_categorical(y_train, num_classes)
    y_test = keras.utils.to_categorical(y_test, num_classes)

    model = Sequential()

    model.add(Conv2D(32, (3, 3), padding='same', input_shape=x_train.shape[1:]))
    model.add(Activation('relu'))

    model.add(MaxPooling2D(pool_size=(2, 2)))
    model.add(Dropout(0.25))

    model.add(Conv2D(64, (3, 3), padding='same'))
    model.add(Activation('relu'))

    model.add(MaxPooling2D(pool_size=(2, 2)))
    model.add(Dropout(0.25))

    model.add(Flatten())

    model.add(Dense(512))
    model.add(Activation('relu'))
    model.add(Dropout(0.5))

    model.add(Dense(num_classes))
    model.add(Activation('softmax'))

    model.summary()

    # initiate RMSprop optimizer
    opt = keras.optimizers.rmsprop(lr=0.001, decay=1e-6)

    # train the model using RMSprop
    model.compile(loss='categorical_crossentropy', optimizer=opt, metrics=['accuracy'])

    hist = model.fit(x_train, y_train, epochs=50, shuffle=True)
    # model.save(model_name)

    # evaluate
    loss, accuracy = model.evaluate(x_test, y_test)
    print("The accuracy of this model is {}%".format(accuracy) )

if __name__=="__main__":
    # compress()
    '''1202 t0 1856 t1
        755 v0 1179 v1
        319 test0   293 test1
        306.7839186191559'''
    start = time.time()
    # X_train, X_test, Y_train, Y_test = readtrain()
    X_train, X_test, Y_train, Y_test = readValid()
    # X_train, X_test, Y_train, Y_test = readtftrain()
    # X_train, X_test, Y_train, Y_test = readtfValid()
    # CNN(X_train, X_test, Y_train, Y_test)



    # svm(X_train, X_test, Y_train, Y_test)
    knn(X_train, X_test, Y_train, Y_test)
    end = time.time()
    print("The total running time is {}".format(end - start))

