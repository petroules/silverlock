package com.petroules.silverlock;

import java.security.*;
import javax.crypto.*;

public class DatabaseCrypto
{
    public static void test() throws NoSuchAlgorithmException, InvalidKeyException
    {
        // Generate secret key for HMAC-MD5
        KeyGenerator kg = KeyGenerator.getInstance("HmacMD5");
        SecretKey sk = kg.generateKey();

        // Get instance of MAC object implementing HMAC-MD5, and
        // initialize it with the above secret key
        Mac mac = Mac.getInstance("HmacMD5");
        mac.init(sk);
        byte[] result = mac.doFinal("Hi There".getBytes());
    }
}
