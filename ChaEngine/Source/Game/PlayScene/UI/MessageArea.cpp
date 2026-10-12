#include "MessageArea.h"
#include "MessageArea/MessageText.h"
#include "MessageArea/MessageAuthor.h"

void MessageArea::Start()
{
	transform.scale = scale;
	transform.position = begin;

	// メッセージテキストのオブジェクトを作る
	objMessageText = GetScene().Instantiate<MessageText>();
	objMessageAuthor = GetScene().Instantiate<MessageAuthor>();
	
	image.texture = "UI/Image/messageframe_41/material/03.png";
}

void MessageArea::Update()
{
	// テキスト本体のオブジェクト参照
	MessageText& messageText = Get<MessageText>(objMessageText);
	
	if (!enabled)
	{
		Hide();
		return;  // もし無効なら非表示にして無視
	}

	if (timeLeft <= 0.0f)
	{
		if (target.isPressedThisFrame)
		{
			// 表示しきった状態でクリックされたら無効化する
			enabled = false;
		}

		return;  // 0.0以下なら無視
	}
	timeLeft -= Get<DeltaTime>();

	if (target.isPressedThisFrame)
	{
		// タイピング中にクリックされたら全文字を強制表示する
		timeLeft = 0.0f;
	}

	if (timeLeft <= 0.0f)
	{
		// メッセージ表示終了
		timeLeft = 0.0f;  // 0未満なら0に補正
		isTyping = false;
	}

	// インデックスを取得する
	int32_t index = static_cast<int32_t>((totalTyepingTime - timeLeft) / charTimeSec);

	// indexを範囲内に収める
	index = std::clamp(index, 0, static_cast<int32_t>(characterIndeces.size() - 1));

	// 表示する文字列の範囲を決定する
	messageText.Data().text.content = showText.substr(0, characterIndeces[index]);
}

void MessageArea::ShowText(const std::string& _str, const std::string& _author)
{
	MessageAuthor& messageAuthor = Get<MessageAuthor>(objMessageAuthor);
	messageAuthor.Data().text.content = _author;

	showText = _str;  // テキストをセット

	// TODO: これはテキストUtilitiesに分けるべき

	// 文字の表示インデックスを入れていく
	// NOTE: 文字コードは必ずしも1文字1byteとは限らないため0~Nbyte目を文字数で区切っていく
	//characterIndeces.clear();
	//for (size_t i = 0; i < _str.length(); i++)
	//{
	//	if ((_str[i] >= 0xD800 && _str[i] <= 0xDBFF) && (i + 1 < _str.length()))
	//	{
	//		// サロゲートペアなので2文字分切り出す
	//		characterIndeces.push_back(i + 2);
	//		i++;
	//	}
	//	else
	//	{
	//		characterIndeces.push_back(i + 1);
	//	}
	//}

	// 文字の表示インデックスを入れていく
	characterIndeces.clear();
	for (size_t i = 0; i < _str.length(); )
	{
		// 現在の文字が何バイトで構成されているかを判定
		unsigned char c = static_cast<unsigned char>(_str[i]);
		size_t charLength = 1;

		if (c >= 0xf0)      charLength = 4; // 絵文字など
		else if (c >= 0xe0) charLength = 3; // 日本語の漢字・ひらがななど
		else if (c >= 0xc0) charLength = 2; // 一部の外国語など
		else                charLength = 1; // 半角英数字など

		// 文字列の末尾を飛び出さないように安全弁を入れる
		if (i + charLength > _str.length()) {
			charLength = _str.length() - i;
		}

		// 次の文字の開始位置（またはこの文字の終了位置）を記録
		i += charLength;
		characterIndeces.push_back(i);
	}

	isTyping = true;  // タイミングを始める
	enabled = true;   // 有効化

	// 総再生時間
	totalTyepingTime = charTimeSec * static_cast<float>(characterIndeces.size());
	timeLeft = totalTyepingTime;

	Show();
}

void MessageArea::Hide()
{
	MessageText& messageText = Get<MessageText>(objMessageText);
	MessageAuthor& messageAuthor = Get<MessageAuthor>(objMessageAuthor);

	transform.scale.x = 0.0f;
	messageText.Data().text.content.clear();
	messageAuthor.Data().text.content.clear();
}

void MessageArea::Show()
{
	transform.scale.x = scale.x;
}
