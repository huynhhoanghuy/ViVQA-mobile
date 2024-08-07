package org.pytorch.helloworld;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.OutputStreamWriter;


import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;

import org.pytorch.helloworld.databinding.ActivityMainBinding;


import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import android.Manifest;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.core.content.FileProvider;

//import com.github.RDRsegmenter.Tokenizer;
//import com.example.rdrsegmenterlib.*;
//import com.github.RDRsegmenter.*;

import androidx.camera.core.*;
import androidx.camera.lifecycle.ProcessCameraProvider;
import androidx.camera.view.PreviewView;
import androidx.core.content.ContextCompat;

import com.google.common.util.concurrent.ListenableFuture;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;



public class MainActivity extends AppCompatActivity {
  private static final String TAG = "MainActivity";
  private ActivityMainBinding binding;
  private String selectedImagePath;
  private static final int STORAGE_PERMISSION_CODE = 1;
  private static final int REQUEST_IMAGE_CAPTURE = 1;
  private static final int REQUEST_CAMERA_PERMISSION = 200;
  private int checkDoneText = 0;
  private String img_path = "";
  private List<String> text_list = new ArrayList<>();
  private List<String> img_path_list = new ArrayList<>();
  private Uri selectedUri;
  private ImageView outputImage;
  private Button singleImagePickerBtn;
  private Button showButton;
  private TextView warningTextView;
  private String segmentedText = "";
  private Button takePictureBtn;
  private Uri photoURI;
  private boolean isInitialized = false;
  private PreviewView previewView;
  private ListenableFuture<ProcessCameraProvider> cameraProviderFuture;
  private ImageCapture imageCapture;
  private final Executor executor = Executors.newSingleThreadExecutor();
  private View cameraControls;
  private FrameLayout cameraContainer;
  private Button captureButton;
  private ProcessCameraProvider cameraProvider;
  private FrameLayout loadingContainer;






  private final ActivityResultLauncher<Intent> takePictureLauncher = registerForActivityResult(
          new ActivityResultContracts.StartActivityForResult(), result -> {
            if (result.getResultCode() == RESULT_OK) {
              if (photoURI != null) {
                processAndDisplayImage(photoURI);
              } else {
                Log.e(TAG, "photoURI is null");
                Toast.makeText(this, "Failed to capture image", Toast.LENGTH_SHORT).show();
              }
            }
          });


  private final ActivityResultLauncher<String> singlePhotoPickerLauncher = registerForActivityResult(
          new ActivityResultContracts.GetContent(),
          uri -> {
            if (uri != null) {
              selectedUri = uri;
              runOnUiThread(() -> outputImage.setImageURI(selectedUri));
              img_path = uri.toString();
              if (!img_path.isEmpty()) {
                img_path_list.add(img_path);
                if (!img_path_list.isEmpty() && !text_list.isEmpty()) {
                  binding = ActivityMainBinding.inflate(getLayoutInflater());
                }
              }
              selectedImagePath = copyFileFromUri(uri);
            }
          }
  );

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    loadingContainer = findViewById(R.id.loadingContainer);


//    previewView = findViewById(R.id.previewView); // Thêm PreviewView vào layout
    cameraProviderFuture = ProcessCameraProvider.getInstance(this);


    Log.d(TAG, "onCreate: Start");

    initializeUI();
    initializeResources();

    Log.d(TAG, "onCreate: End");
  }

  private void initializeUI() {
    takePictureBtn = findViewById(R.id.takePictureBtn);
    showButton = findViewById(R.id.showInput);
    outputImage = findViewById(R.id.outputImage);
    singleImagePickerBtn = findViewById(R.id.singleImagePickBtn);
    warningTextView = findViewById(R.id.warningTextView);
    cameraContainer = findViewById(R.id.cameraContainer);
    previewView = findViewById(R.id.previewView);
//    cameraControls = findViewById(R.id.cameraControls);
    captureButton = findViewById(R.id.captureButton);


    takePictureBtn.setEnabled(false);
    showButton.setEnabled(false);
    singleImagePickerBtn.setEnabled(false);

    takePictureBtn.setOnClickListener(v -> checkCameraPermissionAndCapture_v2());
    showButton.setOnClickListener(this::handleShowButtonClick);
    singleImagePickerBtn.setOnClickListener(this::handleImagePickerClick);
    captureButton.setOnClickListener(v -> {
      try {
        captureImage();
      } catch (IOException e) {
        throw new RuntimeException(e);
      }
    });

    // Ẩn PreviewView và camera controls ban đầu
    previewView.setVisibility(View.GONE);
//    cameraControls.setVisibility(View.GONE);

  }

  private void startCamera() {
    cameraProviderFuture.addListener(() -> {
      try {
        ProcessCameraProvider cameraProvider = cameraProviderFuture.get();
        bindPreview(cameraProvider);
      } catch (ExecutionException | InterruptedException e) {
        // This should never be reached
        Toast.makeText(this, "Error starting camera " + e.getMessage(), Toast.LENGTH_SHORT).show();
      }
    }, ContextCompat.getMainExecutor(this));
  }

  private void bindPreview(ProcessCameraProvider cameraProvider) {
    try {
      // Unbind all use cases before rebinding
      cameraProvider.unbindAll();

      Preview preview = new Preview.Builder().build();
      CameraSelector cameraSelector = new CameraSelector.Builder()
              .requireLensFacing(CameraSelector.LENS_FACING_BACK)
              .build();

      imageCapture = new ImageCapture.Builder()
              .setCaptureMode(ImageCapture.CAPTURE_MODE_MINIMIZE_LATENCY)
              .build();

      preview.setSurfaceProvider(previewView.getSurfaceProvider());

      Camera camera = cameraProvider.bindToLifecycle(this, cameraSelector, preview, imageCapture);
      this.cameraProvider = cameraProvider;
      runOnUiThread(() -> {
        cameraContainer.setVisibility(View.VISIBLE);
        previewView.setVisibility(View.VISIBLE);
      });
    } catch (Exception e) {
      Log.e(TAG, "Use case binding failed", e);
    }
  }
  private void releaseCamera() {
    if (cameraProvider != null) {
      cameraProvider.unbindAll();
      cameraProvider = null;
    }
  }
  @Override
  public void onBackPressed() {
    super.onBackPressed();
    releaseCamera();
  }







  private void captureImage() throws IOException {
    File photoFile = null;

    try {
      photoFile = createImageFile();
    } catch (IOException ex) {
      Log.e(TAG, "Error creating image file", ex);
      Toast.makeText(this, "Error creating image file", Toast.LENGTH_SHORT).show();
      return;
    }

    ImageCapture.OutputFileOptions outputFileOptions =
            new ImageCapture.OutputFileOptions.Builder(photoFile).build();

    File finalPhotoFile = photoFile;
    imageCapture.takePicture(outputFileOptions, executor,
            new ImageCapture.OnImageSavedCallback() {
              @Override
              public void onImageSaved(ImageCapture.OutputFileResults outputFileResults) {
                runOnUiThread(() -> {
                  Toast.makeText(MainActivity.this, "Image captured successfully", Toast.LENGTH_SHORT).show();
                  Uri savedUri = Uri.fromFile(finalPhotoFile);
                  processAndDisplayImage(savedUri);
//                  previewView.setVisibility(View.GONE);
//                  cameraControls.setVisibility(View.GONE);
//                  captureButton.setVisibility(View.GONE);
                  cameraContainer.setVisibility(View.GONE);
                  outputImage.setVisibility(View.VISIBLE);
                  takePictureBtn.setVisibility(View.VISIBLE);
                  singleImagePickerBtn.setVisibility(View.VISIBLE);
                  showButton.setVisibility(View.VISIBLE);


                });
              }

              @Override
              public void onError(ImageCaptureException error) {
                runOnUiThread(() -> {
                  Toast.makeText(MainActivity.this, "Error capturing image: " + error.getMessage(), Toast.LENGTH_SHORT).show();
                });
              }
            });
  }




  private void initializeResources() {
    runOnUiThread(() -> loadingContainer.setVisibility(View.VISIBLE));

    new Thread(() -> {
      long startTime = System.currentTimeMillis();
      copyAssets();
      initializeModels();
      long endTime = System.currentTimeMillis();
      Log.d(TAG, "Initialization took " + (endTime - startTime) + " ms");
      isInitialized = true;
      runOnUiThread(() -> {
        loadingContainer.setVisibility(View.GONE);
        takePictureBtn.setEnabled(true);
        showButton.setEnabled(true);
        singleImagePickerBtn.setEnabled(true);
      });
    }).start();
  }

  private void initializeModels() {
    String commonModelPath = "/data/data/org.pytorch.helloworld/files/common_stack_input.ptl";
    String commonClassifierModelPath = "/data/data/org.pytorch.helloworld/files/classifier_common.ptl";
    String numberClassifierModelPath = "/data/data/org.pytorch.helloworld/files/number.ptl";
    String colorClassifierModelPath = "/data/data/org.pytorch.helloworld/files/color.ptl";
    NativeInterface.initModel(commonModelPath, commonClassifierModelPath, numberClassifierModelPath, colorClassifierModelPath);
  }

  private void handleShowButtonClick(View view) {
    EditText editText = findViewById(R.id.editText);
    String enteredText = editText.getText().toString().trim();
    if (enteredText.isEmpty() || selectedUri == null) {
      if (enteredText.isEmpty() && selectedUri == null) {
        warningTextView.setText("Please enter some text and select an image before predicting.");
      } else if (enteredText.isEmpty()) {
        warningTextView.setText("Please enter some text before predicting.");
      } else {
        warningTextView.setText("Please select an image before predicting.");
      }
      warningTextView.setVisibility(View.VISIBLE);
      return;
    }

    warningTextView.setVisibility(View.GONE);
    checkDoneText = 1;
    text_list.add(enteredText);

    try {
      segmentedText = enteredText + " ";
      OutputStreamWriter outputStreamWriter = new OutputStreamWriter(openFileOutput("inputSegment.txt", Context.MODE_PRIVATE));
      outputStreamWriter.write(segmentedText);
      outputStreamWriter.close();
    } catch (IOException e) {
      Log.e(TAG, "Error writing to inputSegment.txt", e);
      Toast.makeText(this, "Error preparing text input", Toast.LENGTH_SHORT).show();
      return;
    }

    processInputAndShowResult(enteredText);
  }

  private void processInputAndShowResult(String enteredText) {
    String commonModelPath = "/data/data/org.pytorch.helloworld/files/common_stack_input.ptl";
    String commonClassifierModelPath = "/data/data/org.pytorch.helloworld/files/classifier_common.ptl";
    String numberClassifierModelPath = "/data/data/org.pytorch.helloworld/files/number.ptl";
    String colorClassifierModelPath = "/data/data/org.pytorch.helloworld/files/color.ptl";
    String inputSegmentPath = "/data/data/org.pytorch.helloworld/files/inputSegment.txt";

    String result = NativeInterface.processNpyWithModel(selectedImagePath, inputSegmentPath, commonModelPath, commonClassifierModelPath, numberClassifierModelPath, colorClassifierModelPath);

    showResultDialog(enteredText, result);
  }

  private void showResultDialog(String question, String result) {
    AlertDialog.Builder builder = new AlertDialog.Builder(this);
    View dialogView = LayoutInflater.from(this).inflate(R.layout.dialog_layout, null);

    ImageView dialogImageView = dialogView.findViewById(R.id.dialog_image);
    TextView dialogTextView = dialogView.findViewById(R.id.dialog_text);
    TextView dialogPredictionView = dialogView.findViewById(R.id.dialog_prediction);

    dialogImageView.setImageURI(Uri.parse(selectedImagePath));
    dialogTextView.setText("Question: " + question);
    dialogPredictionView.setText("Predict: " + result);

    builder.setView(dialogView)
            .setPositiveButton("OK", null)
            .show();
  }

  private void handleImagePickerClick(View v) {
    if (ContextCompat.checkSelfPermission(MainActivity.this, Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED) {
      singlePhotoPickerLauncher.launch("image/*");
    } else {
      ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, STORAGE_PERMISSION_CODE);
    }
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    releaseCamera();
  }

  private void checkCameraPermissionAndCapture_v2() {
    if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
      ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA}, REQUEST_CAMERA_PERMISSION);
    } else {
      releaseCamera();
      cameraContainer.setVisibility(View.VISIBLE);
//
      previewView.setVisibility(View.VISIBLE);
//      cameraControls.setVisibility(View.VISIBLE);
      captureButton.setVisibility(View.VISIBLE);
      outputImage.setVisibility(View.GONE);
      takePictureBtn.setVisibility(View.GONE);  // Ẩn nút "Take Picture"
      singleImagePickerBtn.setVisibility(View.GONE);  // Ẩn nút "Single Image Picker"
      showButton.setVisibility(View.GONE);
      startCamera();
    }
  }

  private void checkCameraPermissionAndCapture() {
    if (!isInitialized) {
      Toast.makeText(this, "App is still initializing, please wait...", Toast.LENGTH_SHORT).show();
      return;
    }

    if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
      ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA}, REQUEST_CAMERA_PERMISSION);
    } else {
      dispatchTakePictureIntent();
    }
  }

  private void dispatchTakePictureIntent() {
    Intent takePictureIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
    if (takePictureIntent.resolveActivity(getPackageManager()) != null)
    {
      File photoFile = null;
      try {
        photoFile = createImageFile();
      } catch (IOException ex) {
        Log.e(TAG, "Error creating image file", ex);
        Toast.makeText(this, "Error creating image file", Toast.LENGTH_SHORT).show();
        return;
      }
      if (photoFile != null) {
        try {
          photoURI = FileProvider.getUriForFile(this,
                  "org.pytorch.helloworld.fileprovider",
                  photoFile);
          takePictureIntent.putExtra(MediaStore.EXTRA_OUTPUT, photoURI);
          takePictureLauncher.launch(takePictureIntent);

//          startActivityForResult(takePictureIntent, REQUEST_IMAGE_CAPTURE);
        } catch (IllegalArgumentException e) {
          Log.e(TAG, "Error with FileProvider", e);
          Toast.makeText(this, "Error with FileProvider", Toast.LENGTH_SHORT).show();
        }
      }
    } else {
      Toast.makeText(this, "No camera app available", Toast.LENGTH_SHORT).show();
    }
  }

  @Override
  protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    super.onActivityResult(requestCode, resultCode, data);
    if (requestCode == REQUEST_IMAGE_CAPTURE && resultCode == RESULT_OK) {
      if (photoURI != null) {
        processAndDisplayImage(photoURI);
      } else {
        Log.e(TAG, "photoURI is null");
        Toast.makeText(this, "Failed to capture image", Toast.LENGTH_SHORT).show();
      }
    }
  }

  private void processAndDisplayImage(Uri imageUri) {
    Log.d(TAG, "Processing image started");
    runOnUiThread(() -> Toast.makeText(MainActivity.this, "Processing image...", Toast.LENGTH_SHORT).show());

    new Thread(() -> {
      try {
        long startTime = System.currentTimeMillis();

        InputStream inputStream = getContentResolver().openInputStream(imageUri);
        Bitmap bitmap = BitmapFactory.decodeStream(inputStream);
        if (inputStream != null) inputStream.close();

        bitmap = resizeBitmap(bitmap, 1024, 1024);

        File outputFile = new File(getFilesDir(), "input_image.png");
        FileOutputStream fos = new FileOutputStream(outputFile);
        bitmap.compress(Bitmap.CompressFormat.PNG, 90, fos);
        fos.close();

        selectedImagePath = outputFile.getAbsolutePath();
        selectedUri = Uri.fromFile(outputFile);
        img_path = selectedUri.toString();

        if (!img_path.isEmpty()) {
          img_path_list.add(img_path);
        }

        long endTime = System.currentTimeMillis();
        Log.d(TAG, "Image processing took " + (endTime - startTime) + " ms");

        Bitmap finalBitmap = bitmap;
        runOnUiThread(() -> {
          outputImage.setImageBitmap(finalBitmap);
          Toast.makeText(MainActivity.this, "Image processed successfully", Toast.LENGTH_SHORT).show();
        });

      } catch (IOException e) {
        Log.e(TAG, "Error processing image", e);
        runOnUiThread(() -> Toast.makeText(MainActivity.this, "Error processing image", Toast.LENGTH_SHORT).show());
      }
    }).start();
  }

  private Bitmap resizeBitmap(Bitmap bitmap, int maxWidth, int maxHeight) {
    int width = bitmap.getWidth();
    int height = bitmap.getHeight();
    float ratio = Math.min((float) maxWidth / width, (float) maxHeight / height);
    int newWidth = Math.round(width * ratio);
    int newHeight = Math.round(height * ratio);
    return Bitmap.createScaledBitmap(bitmap, newWidth, newHeight, true);
  }

  private File createImageFile() throws IOException {
    String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());
    String imageFileName = "JPEG_" + timeStamp + "_";
    File storageDir = getExternalFilesDir(Environment.DIRECTORY_PICTURES);
    File image = File.createTempFile(
            imageFileName,
            ".jpg",
            storageDir
    );
    selectedImagePath = image.getAbsolutePath();
    return image;
  }

  private String copyFileFromUri(Uri uri) {
    String fileName = "input_image.png";
    String filePath = getApplicationContext().getFilesDir() + "/" + fileName;

    try {
      InputStream inputStream = getContentResolver().openInputStream(uri);
      FileOutputStream outputStream = new FileOutputStream(filePath);
      byte[] buffer = new byte[1024];
      int length;

      while ((length = inputStream.read(buffer)) > 0) {
        outputStream.write(buffer, 0, length);
      }

      outputStream.close();
      inputStream.close();
    } catch (IOException e) {
      Log.e(TAG, "Error copying file from URI", e);
    }

    return filePath;
  }


  private void copyAssets() {
    AssetManager assetManager = getAssets();
    String[] files = {"common_stack_input.ptl", "classifier_common.ptl", "number.ptl", "color.ptl",
            "bpe.codes", "inputSegment.txt", "Model.RDR", "outputFastBPE.txt", "tokenizer.json", "VnVocab", "vocab.txt"};

    for (String filename : files) {
      InputStream in = null;
      OutputStream out = null;
      try {
        in = assetManager.open(filename);
        File outFile = new File(getApplicationContext().getFilesDir(), filename);
        out = new FileOutputStream(outFile);
        copyFile(in, out);
      } catch(IOException e) {
        Log.e("tag", "Failed to copy asset file: " + filename, e);
      } finally {
        if (in != null) {
          try {
            in.close();
          } catch (IOException e) {
// NOOP
          }
        }
        if (out != null) {
          try {
            out.close();
          } catch (IOException e) {
// NOOP
          }
        }
      }
    }
  }

  private void copyFile(InputStream in, OutputStream out) throws IOException {
    byte[] buffer = new byte[1024];
    int read;
    while((read = in.read(buffer)) != -1){
      out.write(buffer, 0, read);
    }
  }
  @Override
  public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
    super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    if (requestCode == REQUEST_CAMERA_PERMISSION) {
      if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
        startCamera();
      } else {
        Toast.makeText(this, "Camera permission is required to take pictures", Toast.LENGTH_SHORT).show();
      }
    }
  }


}
