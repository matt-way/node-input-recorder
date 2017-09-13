{
  "targets": [
    {
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "target_name": "input-recorder",
      "sources": [ "addon/input-recorder.cpp" ]
    }
  ]
}