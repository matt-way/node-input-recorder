{
  "targets": [
    {
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "target_name": "input-recorder",
      "sources": [ "addon/input-recorder.cpp" ],
      'conditions': [
        ['OS == "mac"', {
          'include_dirs': [
            'System/Library/Frameworks/ApplicationServices.framework/Headers'
          ],
          'link_settings': {
            'libraries': [
              '-framework ApplicationServices'
            ]
          }
        }]
      ]      
    }
  ]
}