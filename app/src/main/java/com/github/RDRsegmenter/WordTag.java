package com.github.RDRsegmenter;

/**
 * @author DatQuocNguyen
 * 
 */
public class WordTag
{
    public String word;
    public String tag;
    public String form;

    public WordTag(String iword, String itag)
    {
        form = iword;
        word = iword.toLowerCase();
        tag = itag;
    }
}
