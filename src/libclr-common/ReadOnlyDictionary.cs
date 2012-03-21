namespace Petroules.Silverlock
{
    using System;
    using System.Collections.Generic;

    // http://stackoverflow.com/questions/678379/is-there-a-read-only-generic-dictionary-available-in-net/1269311#1269311
    public class ReadOnlyDictionary<TKey, TValue> : IDictionary<TKey, TValue>
    {
        private IDictionary<TKey, TValue> dictionary;

        public ReadOnlyDictionary(IDictionary<TKey, TValue> dictionary)
        {
            this.dictionary = dictionary;
        }

        #region IDictionary<TKey,TValue> Members

        public void Add(TKey key, TValue value)
        {
            throw new NotSupportedException("This dictionary is read-only");
        }

        public bool ContainsKey(TKey key)
        {
            return this.dictionary.ContainsKey(key);
        }

        public ICollection<TKey> Keys
        {
            get { return this.dictionary.Keys; }
        }

        public bool Remove(TKey key)
        {
            throw new NotSupportedException("This dictionary is read-only");
        }

        public bool TryGetValue(TKey key, out TValue value)
        {
            return this.dictionary.TryGetValue(key, out value);
        }

        public ICollection<TValue> Values
        {
            get { return this.dictionary.Values; }
        }

        public TValue this[TKey key]
        {
            get
            {
                return dictionary[key];
            }

            set
            {
                throw new NotSupportedException("This dictionary is read-only");
            }
        }

        #endregion

        #region ICollection<KeyValuePair<TKey,TValue>> Members

        public void Add(KeyValuePair<TKey, TValue> item)
        {
            throw new NotSupportedException("This dictionary is read-only");
        }

        public void Clear()
        {
            throw new NotSupportedException("This dictionary is read-only");
        }

        public bool Contains(KeyValuePair<TKey, TValue> item)
        {
            return this.dictionary.Contains(item);
        }

        public void CopyTo(KeyValuePair<TKey, TValue>[] array, int arrayIndex)
        {
            this.dictionary.CopyTo(array, arrayIndex);
        }

        public int Count
        {
            get { return this.dictionary.Count; }
        }

        public bool IsReadOnly
        {
            get { return true; }
        }

        public bool Remove(KeyValuePair<TKey, TValue> item)
        {
            throw new NotSupportedException("This dictionary is read-only");
        }

        #endregion

        #region IEnumerable<KeyValuePair<TKey,TValue>> Members

        public IEnumerator<KeyValuePair<TKey, TValue>> GetEnumerator()
        {
            return this.dictionary.GetEnumerator();
        }

        #endregion

        #region IEnumerable Members

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            return (this.dictionary as System.Collections.IEnumerable).GetEnumerator();
        }

        #endregion
    }
}
