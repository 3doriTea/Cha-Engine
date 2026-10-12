
template<typename Func>
inline void StageLoader::OnSetBlock(Func&& _func)
{
	_func();
}
