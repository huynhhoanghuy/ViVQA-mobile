package org.pytorch.helloworld;

public class NativeInterface {
    static {
        System.loadLibrary("pytorch_nativeapp_VQA");
    }

    public static native void initModel(String commonModelPath, String commonClassifierModelPath,
                                        String numberClassifierModelPath, String colorClassifierModelPath);

    //    public static native float[] processNpyWithModel(String npyPath, String modelPath);
    public static native String processNpyWithModel(String imagePath, String segmentedText, String modelPath, String classifier_modelPath, String number_modelPath, String color_modelPath);
}
