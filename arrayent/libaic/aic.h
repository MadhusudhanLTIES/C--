/**
 *
 * @File   aic.h
 * @Brief  Shared library header for arrayent internet connect
 *         service daemon (aicd) interface.
 *
 * Copyright (c) 2012 Arrayent Inc.  Company confidential.
 * Please contact sales@arrayent.com to get permission to use in your
 * application.
 *
 * Author               Date    	Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Dustin McIntire		02/27/2012	Original version released
 *
 **/

#ifndef AIC_H_
#define AIC_H_

/**  Initialize the Arrayent API.
 * This function prepares the Arrayent API library for communication
 * with the Arrayent server.
 *
 * Call this function once during system initialization.
 * It must be called after the Arrayent routing daemon has started.
 *
 * @return    Indicates if initialization was completed successfully
 *            Returns 0 on success or -1 on failure
 */
int ArrayentInit(void);

/**  Initialize the Arrayent API.
 * This function prepares the Arrayent API library for communication
 * with the Arrayent server and is different than the legacy
 * ArrayentInit in that it takes an argument to attached to  a
 * specific instance of the aicd daemon that is started with
 * a --id='some_text' argument.
 *
 * Call this function once during system initialization.
 * It must be called after the Arrayent routing daemon has started with
 * a caller generated unique identifier
 *
 * @return    Indicates if initialization was completed successfully
 *            Returns 0 on success or -1 on failure
 */
int ArrayentInitWithId(char *id);

/**  Reset the Arrayent Interface.
 * This function forces the routing deamon's connection state machine to
 * reset and log back into the server.  It may be called at any time after
 * initialization is complete.
 *
 * @return    Indicates if the reset was accepted.
 *            Returns 0 on success or -1 on failure
 */
int ArrayentReset(void);

/**  Check connection to the Arrayent routing daemon.
 * This function sends a hello message to the Arrayent routing daemon.  If
 * the connection is functioning correctly the hello message will receive
 * a response message from the routing daemon to indicate a working connection
 *
 * @return    Indicates if the hello was verified.
 *            Returns 0 on success or -1 on failure
 */
int ArrayentHello(void);

/** Network status polling function.
 *  The get network status function returns a bit field indicating network
 *  status of the Arrayent stack..
 *
 * @param status Pointer to the status word to be updated.  The bits in the
 *               status word will be set according to connection state.  The
 *               bits have the following significance
 *
 *     Bit 0: LAN Connected
 *         0 = no RF link
 *         1 = RF link to AP established
 *     Bit 1: IP Address
 *         0 = no IP address found
 *         1 = local IP address has been established
 *     Bit 2: DNS Lookup
 *         0 = DNS lookup for server IP failed
 *         1 = DNS Lookup for server IP has completed
 *     Bit 3: Server IP
 *         0 = No server IP found
 *         1 = IP address of the Arrayent server has been found
 *     Bit 4: Heartbeat OK
 *         0 = Heartbeats to server failed
 *         1 = Heartbeats to server are OK
 *     Bit 5: Using UDP
 *         0 = Not using UDP
 *         1 = Connection using UDP
 *     Bit 6: Using TCP
 *         0 = Not using TCP
 *         1 = Connection using TCP
 *     Bit 7: Server Connected
 *         0 = Connection to server failed
 *         1 = Connection to server is OK
 *
 * @return    Indicates if the status value was updated.
 *            Returns 0 on success or -1 on failure
 */
int ArrayentNetStatus(int *status);

/** Connection status bits definition for bitmask used by ArrayentNetStatus */
#define ARRAYENT_HAS_RF_LINK           (1)
#define ARRAYENT_HAS_IP_ADDRESS        (1<<1)
#define ARRAYENT_HAS_DNS_LOOKUP        (1<<2)
#define ARRAYENT_HAS_SERVER_IP         (1<<3)
#define ARRAYENT_HEARTBEAT_OK          (1<<4)
#define ARRAYENT_USING_UDP             (1<<5)
#define ARRAYENT_USING_TCP             (1<<6)
#define ARRAYENT_SERVER_CONNECTED      (1<<7)
#define ARRAYENT_LOGGED_IN             (1<<8)

/** Setup the login credentials to the Arrayent server.
 * This function sets the login ID and password sent to the arrayent servers
 *
 * @param id Pointer to the device login ID.
 *
 * @param pass  Pointer to the device password ID
 *
 * @return    Indicates if the credentials were set correctly.
 *            Returns 0 on success or -1 on failure
 */
int ArrayentSetCredentials(char *id, char *pass);

/** Send binary data with optional timeout.
 * This function sends data messages to the Arrayent servers
 * synchronously.  It is blocking and will not return until a
 * response message has been received from the server or the
 * timeout value expires.
 *
 * @param data Pointer to the data for transmission.
 *
 * @param len  Length of data to send
 *
 * @param timeout  Timeout for transmission to complete.  This
 *                 value is specified in milliseconds.  A timeout value
 *                 of 0 uses the default timeout.  A timeout value < 0
 *                 will wait forever for a response to be returned.
 *
 * @return    Indicates if the data was sent to the server correctly.
 *            Returns 0 on success or -1 on failure, or ETIMEOUT if
 *            the send timed out prior to receiving server confirmation
 */
int ArrayentSendData(char *data, int len, int timeout);

/** Send binary data with optional callback.
 * This function sends data messages to the Arrayent servers
 * asynchronously.  It is nonblocking and will return immediately to the
 * caller before a response message has been received from the server.
 * An optional callback function may be used for delayed confirmation of delivery
 * and reporting of status.  Callback functions for completed transmissions  are
 * automatically deleted from the callback list after execution and do not need
 * to be removed by caller.
 *
 * @param data Pointer to the data for transmission.
 *
 * @param len  Length of data to send
 *
 * @param handle  Caller specified handle to identify the transmission.  This
 *                value is passed to the callback function as the first argument,
 *                but is not otherwise used by the callee
 *
 * @param callback  Caller specified callback function to be called when the server
 *                  notification is complete.  A NULL value for the callback may be
 *                  used to send data with no callback.  The first argument to the
 *                  callback function is the previously specified caller handle id.
 *                  The second argument to the callback function is the completion
 *                  status of the transmission. The status value contains 0 on success
 *                  or -1 on failure
 *
 * @return    Indicates if the data was sent to the routing daemon succesfully.  It does
 *            not indicate the completion of the transmission to the server as this will be
 *            done via the caller's callback function
 *            Returns 0 on success or -1 on failure
 */
int ArrayentSendDataAsync(char *data, int len, void *handle, void (*callback)(void*, int));

/** Cancel callback function.
 * This function cancels a callback function previously registered with the
 * ArrayentSendDataAsync function.  As there is no timeout with the asynchronous
 * transmision method. It is therefore the responsibility of the caller to remove
 * callback functions which exceed the caller's timeout value and need to be discarded.
 *
 * @param handle  Caller specified handle to identify the transmission.
 *
 * @return    Indicates if the handle was deleted correctly.
 *            Returns 0 on success or -1 on failure
 */
int ArrayentCancelCallback(void *handle);

/** Receive binary data with optional timeout.
 * This function receives data messages from the Arrayent servers
 * synchronously.  It is blocking and will not return until a
 * data message has been received from the server or the timeout value
 * expires.
 *
 * @param data Pointer to the buffer to copy binary data into.
 *
 * @param len  Pointer to the length of data to send.  The pointer should
 *             point to an integer containing the callers buffer size.  Upon
 *             successfully receiving a data message, the contents of the
 *             length are updated with the size of the data written to the
 *             data buffer specified by the data parameter above.
 *
 * @param timeout  Timeout for receive operation to complete.  This
 *                 value is specified in milliseconds.  A value of
 *                 0 indicates no wait.
 *
 * @return    Indicates if the data was received from the server correctly.
 *            Returns 0 on success or -1 on failure, or ETIMEOUT if
 *            the receive operation timed out prior to getting server data.
 */
int ArrayentRecvData(char *data, int *len, int timeout);

/** Receive property data with optional timeout.
 * This function receives data messages from the Arrayent servers
 * synchronously.  It is blocking and will not return until a
 * data message has been received from the server or the timeout value
 * expires.
 *
 * @param data Pointer to the buffer to copy property data into.
 *
 * @param len  Pointer to the length of data to send.  The pointer should
 *             point to an integer containing the callers buffer size.  Upon
 *             successfully receiving a data message, the contents of the
 *             length are updated with the size of the data written to the
 *             data buffer specified by the data parameter above.
 *
 * @param timeout  Timeout for receive operation to complete.  This
 *                 value is specified in milliseconds.  A value of
 *                 0 indicates no wait.
 *
 * @return    Indicates if the data was received from the server correctly.
 *            Returns 0 on success or -1 on failure, or ETIMEOUT if
 *            the receive operation timed out prior to getting server data.
 */
int ArrayentRecvProperty(char *data, int *len, int timeout);

/** Write a new property to the server.
 * This function sets an arbitrary property on the Arrayent servers
 *
 * @param property Pointer to the property name.
 *
 * @param value  Pointer to the property value.
 *
 * @return    Indicates if the property was set correctly.
 *            Returns 0 on success or -1 on failure
 */
int ArrayentSetProperty(char *property, char *value);

/** Retrieve a new property from the server.
 * This function gets an arbitrary property from the Arrayent servers
 *
 * @param property Pointer to the property name.
 *
 * @param value  Pointer to the property value buffer to be written.
 *
 * @param len  Size of the buffer pointed to by the value parameter.
 *
 * @return    Indicates if the property was received correctly.
 *            Returns 0 on success or -1 on failure
 */
int ArrayentGetProperty(char *property, char *value, int len);

#endif /* AIC_H_ */
