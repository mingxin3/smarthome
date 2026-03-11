set DASHSCOPE_API_KEY=sk-bb7cd27ea79d4ee489c5237aef25705f

curl https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions ^
  -H "Authorization: Bearer %DASHSCOPE_API_KEY%" ^
  -H "Content-Type: application/json" ^
  -d "{\"model\":\"qwen3.5-plus\",\"messages\":[{\"role\":\"user\",\"content\":\"ping\"}]}"